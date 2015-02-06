#include "asyncSerial.h"
#include <IOKit/serial/ioss.h>
#include <sys/ioctl.h>
#include <libc.h>
#include <pthread.h>
#include <glob.h>


/*------------------------------------------------------------------------------------*/
struct _asyncSerial 
{
	t_object					ob;			// the object itself (must be first)
  
  void              *leftOutlet;
  void              *rightOutlet;
  
  int serialFileDescriptor;
  struct termios oldTerminalAttributes;
  
  int preferredBaud;
  int preferredShouldListen;
  int preferredVmin;
  int preferredVtime;
  int preferredIcanon;
  
  pthread_t listenThread;
  int threadIsRunning;
  int threadShouldContinueRunning;
  
  void (*outputReceivedData)(t_asyncSerial*, void*, int);
};

void *asyncSerial_class;



/*------------------------------------------------------------------------------------*/
int main(void)
{	
	t_class *c;
	
	c = class_new("asyncSerial", (method)asyncSerial_new, (method)asyncSerial_free, (long)sizeof(t_asyncSerial), 
				  0L /* leave NULL!! */, A_GIMME, 0);
	
  class_addmethod(c, (method)asyncSerial_assist        , "assist"    , A_CANT , 0);  
  class_addmethod(c, (method)asyncSerial_open          , "open"      , A_SYM  , 0);
  class_addmethod(c, (method)asyncSerial_close         , "close"     ,          0);
  class_addmethod(c, (method)asyncSerial_setBaud       , "baud"      , A_LONG,  0);
  class_addmethod(c, (method)asyncSerial_flush         , "flush"     ,          0);
  class_addmethod(c, (method)asyncSerial_listen        , "listen"    , A_LONG , 0);
  class_addmethod(c, (method)asyncSerial_setInputMode  , "input_mode", A_LONG , A_LONG, A_LONG, 0);
  class_addmethod(c, (method)asyncSerial_setOutputMode , "output_mode", A_LONG , 0);
  class_addmethod(c, (method)asyncSerial_print         , "print"     , A_GIMME, 0);
  class_addmethod(c, (method)asyncSerial_int           , "int"       , A_LONG , 0);
  class_addmethod(c, (method)asyncSerial_printPorts     , "ports"     ,         0); 
    
  class_register(CLASS_BOX, c); /* CLASS_FAILUREBOX */
	asyncSerial_class = c;

	return 0;
}

/*------------------------------------------------------------------------------------*/
void *asyncSerial_new(t_symbol *s, long argc, t_atom *argv)
{
	t_asyncSerial *x = NULL;    
  
  signal(SIGUSR1, asyncSerial_signalHandler);
  
	if ((x = (t_asyncSerial *)object_alloc(asyncSerial_class))) 
    {
        x->rightOutlet          = intout(x);
        x->leftOutlet           = outlet_new(x, NULL);
        
        x->preferredBaud        = 9600;
        x->preferredShouldListen= 1;
        x->preferredVmin        = 0;
        x->preferredVtime       = 0;
        x->preferredIcanon      = 0;
        
        x->serialFileDescriptor = -1;
        x->threadIsRunning      = 0;
        x->threadShouldContinueRunning = 0;
        
        x->outputReceivedData = asyncSerial_binOut;
  
        if(argc > 2)
          object_post((t_object *)x, "excess arguments");
        else
          {
            while (argc)
              {
                //error checking for order of arguements could be refined
                if(argv->a_type == A_SYM)
                  {
                    asyncSerial_open(x, atom_getsym(argv));
                  }
                else if(argv->a_type == A_LONG)
                  {
                    asyncSerial_setBaud(x, atom_getlong(argv));
                  }
                else
                  object_post((t_object *)x, "Invalid arguemnt. Only pass the optional path of the serial port to use followed by the optional baud rate");
                  
                argc--;
                argv++;
              }
              
          }
	  }
	return (x);
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_free(t_asyncSerial *x)
{
	asyncSerial_close(x);
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_assist(t_asyncSerial *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) 
    { // inlet
      if     (a == 0)
		    sprintf(s, "inlet %ld: Messages. print sends as ASCII, int sends as binary", a);
	  } 
	else 
    {	// outlet
      if     (a == 0)
        sprintf(s, "outlet %ld: Data received from serial port", a); 			
      else if(a == 1)
        sprintf(s, "outlet %ld: Data sent to serial port. For debugging", a); 
	  }
}

/*------------------------------------------------------------------------------------*/
success_t asyncSerial_open(t_asyncSerial *x, t_symbol* sym)
{
	int error;
	unsigned long latency = 1;  //microseconds
	struct termios newTerminalAttributes;
  char* portname = sym->s_name;
    
  if(x->serialFileDescriptor != -1)
    {
      asyncSerial_close(x);
      sleep(1);
    }  
  
	x->serialFileDescriptor = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if(x->serialFileDescriptor == -1) {  object_post((t_object *)x, "Cannot open %s\n", portname); return FAILURE;  };
	
	error = ioctl(x->serialFileDescriptor, TIOCEXCL);
	if(error == -1) {  object_post((t_object *)x, "Error opening serial port %s\n", portname); return FAILURE;  }
	
	fcntl(x->serialFileDescriptor, F_SETFL, 0);
	
	error = tcgetattr(x->serialFileDescriptor, &(x->oldTerminalAttributes));
	if(error == -1) {  object_post((t_object *)x, "Error getting serial terminal attributes\n"); return FAILURE;  }
	
	newTerminalAttributes = x->oldTerminalAttributes; 
	
	cfmakeraw(&newTerminalAttributes);
	
	error = tcsetattr(x->serialFileDescriptor, TCSANOW, &newTerminalAttributes);
	if(error == -1) {  object_post((t_object *)x, "Error setting serial attributes\n"); return FAILURE;  }
	
  error = asyncSerial_setInputMode(x, x->preferredVtime, x->preferredVmin, x->preferredIcanon);
  
  
	error = asyncSerial_setBaud(x, x->preferredBaud);
	
	error = ioctl(x->serialFileDescriptor, IOSSDATALAT, &latency);
	
  error = asyncSerial_flush(x);
  
  asyncSerial_listen(x, x->preferredShouldListen);

	object_post((t_object *)x, "Opened serial connection to %s\n", portname);
	
	return SUCCESS;
}


/*------------------------------------------------------------------------------------*/
void asyncSerial_close(t_asyncSerial *x)
{
  if(x->serialFileDescriptor != -1)
    { 
      int shouldListen = x->preferredShouldListen;
      asyncSerial_listen(x, false);
      x->preferredShouldListen = shouldListen;
      tcsetattr(x->serialFileDescriptor, 0, &(x->oldTerminalAttributes));
      close(x->serialFileDescriptor);
      x->serialFileDescriptor = -1;
    }
}

/*------------------------------------------------------------------------------------*/
success_t asyncSerial_setBaud(t_asyncSerial *x, int baud)
{
  int error;
  int previousBaud = x->preferredBaud;
  x->preferredBaud = baud; 
  
  if(x->serialFileDescriptor != -1)
    { 
      error = ioctl(x->serialFileDescriptor, IOSSIOSPEED, &baud);
	    if(error == -1)
        {
	        object_post((t_object *)x, "Error setting serial baud rate\n" );  
          x->preferredBaud = previousBaud; 
        }
    }
  return SUCCESS;
}


/*------------------------------------------------------------------------------------*/
success_t asyncSerial_flush(t_asyncSerial *x)
{
  int error = SUCCESS;
  if(x->serialFileDescriptor != -1)
    {
	    int  whatToFlush = FREAD;
	    error = ioctl(x->serialFileDescriptor, TIOCFLUSH, &whatToFlush);
      error = !(error == -1);
      if(error == FAILURE) object_post((t_object *)x, "Unable to flush serial input buffer\n");	
    }
  return error;
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_int(t_asyncSerial *x, long n)
{
  unsigned long N = (unsigned long)n;
  int size = sizeof(long);
  unsigned char data[size];
  unsigned char* p = data+size-1;
  int numBytes = 0;
  do{
      *p-- = N & 0xFFL;
      N = N >> 8;
      numBytes++;
    }while (N != 0);
    
  asyncSerial_write (x, data + size - numBytes, numBytes);
}

/*------------------------------------------------------------------------------------*/
int asyncSerial_write (t_asyncSerial *x, void* data, int numChars)
{
  int i;
  for(i=0; i<numChars; i++)
    {
      outlet_int(x->rightOutlet, (long)*(((unsigned char*) data) + i));
    }
  return write(x->serialFileDescriptor, (void*)data, numChars);
}

/*------------------------------------------------------------------------------------*/
int asyncSerial_read(t_asyncSerial *x, void* data, int numChars)
{
  return read(x->serialFileDescriptor, data, numChars);
}


/*------------------------------------------------------------------------------------*/
void asyncSerial_print(t_asyncSerial *x, t_symbol *s, long argc, t_atom *argv)
{
  long i;
  t_atom *ap;
  char* data;
  int numBytes;
  
  for (i = 0, ap = argv; i < argc; i++, ap++) 
    {
      switch (atom_gettype(ap)) 
        {
          case A_LONG:
            asprintf(&data, "%li ", atom_getlong(ap));
            break;
          case A_FLOAT:
            asprintf(&data, "%f ", atom_getfloat(ap));
            break;
          case A_SYM:
            asprintf(&data, "%s ", atom_getsym(ap)->s_name);
            break;
          default:
            object_post((t_object *)x, "%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
            break;
         }
         
       numBytes = strlen(data);
       if(i==argc-1) numBytes--;
       asyncSerial_write(x, data, numBytes);  
       free(data);
    }
}


/*------------------------------------------------------------------------------------*/
success_t asyncSerial_setInputMode(t_asyncSerial* x, int vtime, int vmin, bool icanon)
{
  int error = -1;
  int previousVtime  = x->preferredVtime;
  int previousVmin   = x->preferredVmin;
  int previousIcanon = x->preferredIcanon;
  x->preferredVtime  = vtime;
  x->preferredVmin   = vmin;
  x->preferredIcanon = icanon;
  
  if(x->serialFileDescriptor != -1)
    {
      struct termios attributes;
      if(tcgetattr(x->serialFileDescriptor, &attributes) != -1)
        {
          attributes.c_cc[VTIME] = vtime;
	        attributes.c_cc[VMIN ] = vmin ;
          if(icanon == true) attributes.c_lflag |= ICANON;
          else attributes.c_lflag &= ~ICANON;
          error = tcsetattr(x->serialFileDescriptor, TCSANOW, &attributes);
          if(error == -1)
            {
              x->preferredVtime  = previousVtime;
              x->preferredVmin   = previousVmin;
              x->preferredIcanon = previousIcanon; 
              object_post((t_object *)x, "Unable to set serial input mode\n");	           
            }
        }
    }
  return error != -1;
}


/*------------------------------------------------------------------------------------*/
success_t asyncSerial_listen(t_asyncSerial *x, bool shouldListen)
{
  x->preferredShouldListen = shouldListen;
  if(shouldListen)
    {
      if((x->serialFileDescriptor != -1) && (x->threadIsRunning != 1))
        {
          x->threadIsRunning = 1;
          pthread_create(&(x->listenThread), NULL, asyncSerial_listenThread, (void*) x);
        }
    }
  else //!shouldListen
    {
      if(x->threadIsRunning == 1)
        {
          x->threadShouldContinueRunning = 0;
          pthread_kill(x->listenThread, SIGUSR1);
          x->threadIsRunning = 0;
        }
    }
  return SUCCESS; 
}


/*------------------------------------------------------------------------------------*/
void* asyncSerial_listenThread(void *_x)
{
  post("listen entered");
  t_asyncSerial* x = (t_asyncSerial*)_x;
  
  x->threadShouldContinueRunning = 1;
  int numBytesRead;
  char buffer[101];
  
  while(x->threadShouldContinueRunning == 1)
  {
    numBytesRead = asyncSerial_read (x, buffer, 100);
    if(numBytesRead < 0) //error or signal
      break;
    else if(numBytesRead >= 1)
      x->outputReceivedData(x, buffer, numBytesRead);
    else //0 bytes but no error
      usleep(500);
      continue;
  }
  post("listen exited");
  return NULL;
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_binOut(t_asyncSerial *x, void* buffer, int numValidBytes)
{
  unsigned char* b = (unsigned char*)buffer;
  while(numValidBytes-- > 0)
    outlet_int(x->leftOutlet, *b++); 
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_asciiOut(t_asyncSerial *x, void* buffer, int numValidBytes)
{
  char* b = (char*)buffer;
  
  int i, numAtoms = 1;
  for(i=0; i<numValidBytes; i++)
    if(*b++ == ' ')
      {
        numAtoms++;
        b[-1] = '\0';
      }
    
    *b = '\0';
    
    t_atom argv[numAtoms];
    b = buffer;
    for(i=0; i<numAtoms; i++)
      {
        atom_setsym(argv + i, gensym(b));
        b += strlen(b) + 1;
      }
      
    outlet_anything(x->leftOutlet, atom_getsym(argv), numAtoms-1, argv+1);
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_setOutputMode(t_asyncSerial *x, bool isBinary)
{
  if(isBinary)
    x->outputReceivedData = asyncSerial_binOut;
  else
    x->outputReceivedData = asyncSerial_asciiOut;
}

/*------------------------------------------------------------------------------------*/
void asyncSerial_signalHandler(int sig)
{
  if(sig == SIGUSR1)
    return;
  return;
}

void asyncSerial_printPorts(t_asyncSerial *x)
{
  glob_t g;
  if (glob("/dev/cu*", 0, NULL, &g) == 0)
    {
      int i;
      for(i=0; i<g.gl_pathc; i++)
       {
         post("%s\n", g.gl_pathv[i]);
       }
    }
}
