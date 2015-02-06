/*!
 * @header AsyncSerial
 * @abstract <span style="font-family:monospace"><pre>
 *     _                           ____            _       _ 
 *    / \   ___ _   _ _ __   ___  / ___|  ___ _ __(_) __ _| |
 *   / _ \ / __| | | | '_ \ / __| \___ \ / _ \ '__| |/ _` | |
 *  / ___ \\__ \ |_| | | | | (__   ___) |  __/ |  | | (_| | |
 * /_/   \_\___/\__, |_| |_|\___| |____/ \___|_|  |_|\__,_|_|
 *              |___/    
 * </pre></span>
 * 
 * @version
 *  1.1 beta (November 30th 2012)
 *
 * @discussion
 *  An external object for Max/MSP on Mac OSX that interfaces
 *  with serial devices. This is intended to provide more useful
 *  functionality than the built-in [serial] object. Namely, 
 *  [AsyncSerial] listens for incoming data on a separate thread
 *  and spits it out whenever it arrives. It does not need to be
 *  polled. This is similar to how UDPReceive works. Similar care
 *  must be taken when dealing with multithreaded Max-patches.
 *  [AsyncSerial] can also have its input buffer flushed,
 *  which is necessary when sending commands to a device and 
 *  awaiting a reply. [AsyncSerial] can also be put in and out
 *  of 'canonical' input mode, in which data is only spit out once
 *  a newline char '\n' is received, which is useful for parsing
 *  text-based replies. [AsyncSerial] also allows users to set 
 *  VMIN and VTIME, as described in the unix manual, sub verbo 
 *  'termios':
 *
 *  man termios
 *
 *  [AsyncSerial] cannot be polled using 'bang', because, depending 
 *  on VMIN, VTIME and ICANON, that could cause the main thread to
 *  hang.
 *  
 *  [AsyncSerial] can either output received data as a sequence 
 *   of bytes, as [serial] does, or as a list of text symbols, which
 *   is useful with canonic input processing.
 *
 *  for help using [AsyncSerial] from within Max/MSP, consult 
 *  async-serial.maxhelp, which should have come with this file.
 *
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2012.
 *
 *  mkrzyzan@asu.edu
 *
 * @unsorted
 */

#ifndef __ASYNC_SERIAL__
#define __ASYNC_SERIAL__

#include "ext.h"			  				// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

typedef enum success_enum
{
  FAILURE = 0,
  SUCCESS = 1,
}success_t;

////////////////////////// object struct
typedef struct _asyncSerial t_asyncSerial;

/*! @functiongroup standard Max SDK functions */
/*------------------------------------------------------------------------------------*/
/*!
 * @function   asyncSerial_new
 * @discussion create a new object
 * @param      argv
 *               [AsyncSerial] takes two optional arguments which are the name of the
 *               serial port to open (eg /dev/cu.usb-serial-1234), and the baud rate.
 *               the default baud is 9600, and either arguments can be changed later.
 * @result     the new object
 */
void*     asyncSerial_new           (t_symbol *s, long argc, t_atom *argv);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_free
 * @discussion  destroy object
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 */
void      asyncSerial_free          (t_asyncSerial *x);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_assist
 * @discussion  show help from with Max/MSP
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 */
void      asyncSerial_assist        (t_asyncSerial *x, void *b, long m, long a, char *s);

/*! @functiongroup public to Max/MSP */
/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_open
 * @discussion  If there is already a serial port open, close it and wait one second.
 *              Then attempt to open the specified port.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       name
 *                the path to the serial port, to open (eg /dev/cu.usb-serial-1234)
 * @result      SUCCESS or FAILURE. upon FAILURE, an error message is posted to the 
 *              'Max Window'
 */
success_t asyncSerial_open          (t_asyncSerial *x, t_symbol* name);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_close
 * @discussion  If there is a serial port open, close it and restore its settings so
 *              it can be used by another program.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 */
void      asyncSerial_close         (t_asyncSerial *x);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_print
 * @discussion  Format data as ASCII chars and send it to the serial port. 
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       s
 *                the format string describing the contents of argv
 * @param       argc 
 *                the number of elements in argv
 * @param       argv
 *                the argument vector
 */
void asyncSerial_print(t_asyncSerial *x, t_symbol *s, long argc, t_atom *argv); 

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_int
 * @discussion  Send binary data to the serial device. [AsyncSerial] sends one byte 
 *              at a time, so this method is intended to receive messages in the range
 *              0 ~ 255 Larger values will be sent in newtork byte order (most 
 *              significant bit first), in as few bytes are required t encode the 
 *              entire number. For example 1234 will be sent as [4, 210], because
 *              ((4 << 8) & 210) = 1234. Negative values are sent as 2s compliment 
 *              equivalents.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       n
 *                the data to send
 * 
 */
void asyncSerial_int(t_asyncSerial *x, long n);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_setBaud
 * @discussion  Set the baud rate for communication with the currently open serial 
 *              device, and remember this value so it can be used subsequent calls 
 *              to "asyncSerial_open". The defaut baud rate for a new object is 
 *              9600.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       baud
 *                the baud rate to use. This method does not perform boundry checking
 *                so the user is responsible for passing a meaningful value.
 * @result      SUCCESS or FAILURE. upon FAILURE, an error message is posted to the 
 *              'Max Window'
 */
success_t asyncSerial_setBaud       (t_asyncSerial *x, int baud);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_flush
 * @discussion  Flush the serial receive buffer. This deletes data that has been received
 *              by the operating system but not yet spit out by the [AsyncSerial] object.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @result      SUCCESS or FAILURE. upon FAILURE, an error message is posted to the 
 *              'Max Window'
 */
success_t asyncSerial_flush         (t_asyncSerial *x);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_listen
 * @discussion  Start or stop listening asynchronously for incoming data. New objects
 *              start listening upon creation.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       shouldListen
 *                true to start listening, false to stop.
 * @result      SUCCESS or FAILURE. upon FAILURE, an error message is posted to the 
 *              'Max Window'
 */
success_t asyncSerial_listen        (t_asyncSerial *x, bool shouldListen);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_setInputMode
 * @discussion  set the input processing mode. see the UNIX manual sub verbo 'termios' 
 *              for a more detailed description, and more information about how the 
 *              paramaters interact.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       vtime
 *                milliseconds
 * @param       vmin
 *                byte count
 * @param       icanon
 *                true for canonical input processing, false for non-canonical
 * @result      SUCCESS or FAILURE. upon FAILURE, an error message is posted to the 
 *              'Max Window'
 */
success_t asyncSerial_setInputMode(t_asyncSerial* x, int vtime, int vmin, bool icanon);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_setOutputMode
 * @discussion  set the output function to either asyncSerial_binOut or 
 *              asyncSerial_asciiOut.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       isBinary
 *                1 for asyncSerial_binOut or 0 for asyncSerial_asciiOut;
 */
void asyncSerial_setOutputMode(t_asyncSerial *x, bool isBinary);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_printPorts
 * @discussion  print a list of serial ports to the Max window.
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 */
void asyncSerial_printPorts(t_asyncSerial *x);

/*! @functiongroup private from Max/MSP */
/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_write
 * @discussion  send data to the serial device
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       data
 *                a buffer containing data to be sent
 * @param       numChars
 *                the number of valid bytes in data
 * @result      the number of bytes sucessfully sent to the device
 */
int       asyncSerial_write         (t_asyncSerial *x, void* data, int numChars);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_read
 * @discussion  read data from the serial device
 * @param       x
 *                the [AsyncSerial] object to operate upon.
 * @param       data
 *                a buffer containing data to be sent
 * @param       numChars
 *                the number of valid bytes in data
 * @result      the number of bytes sucessfully read from the device
 */
int       asyncSerial_read          (t_asyncSerial *x, void* data, int numChars);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_listenThread
 * @discussion  the main run loop for listening for incoming serial data.
 */
void*     asyncSerial_listenThread (void          *x);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_signalHandler
 * @discussion  catch signals sent to the serial listen thread, so that the program
 *              does not exit when the tread is killed.
 */
void      asyncSerial_signalHandler(int sig);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_binOut
 * @discussion  send received data out of the left outlet as binary data, ie as sequence
 *              of chars sent out one at a time. This function is not called directly,
 *              but via a pointer that selects the output function.
 */
void asyncSerial_binOut  (t_asyncSerial *x, void* buffer, int numValidBytes);

/*------------------------------------------------------------------------------------*/
/*!
 * @function    asyncSerial_asciiOut
 * @discussion  send received data out of the left outlet as a list of symbols. 
 *              Input is broken into pieces based on the location of spaces, packed
 *              into a list, and sent out. Even numbers are put in the list as
 *              symbols, so in Max, [fromsymbol] has to be used to convert them to 
 *              numbers, if desired. This is particularly useful with canonic input
 *              processing.  This function is not called directly,
 *              but via a pointer that selects the output function.
 * @param       numValidBytes
 *                the total capacity of the buffer must be at least one larger than
 *                the number of valid bytes, so that this method can scribble in '\0'
 *                if necessary.
 */
void asyncSerial_asciiOut(t_asyncSerial *x, void* buffer, int numValidBytes);

#endif //__ASYNC_SERIAL__
