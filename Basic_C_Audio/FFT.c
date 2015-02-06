#include "FFT.h"
#include <math.h>
#include "fastsin.h"

void     fftRawForwardFFT          (FFT* self);
void     fftRawInverseFFT          (FFT* self);
void     fftApplyWindow            (FFT* self);
void     fftShiftSlidingOutput     (FFT* self);
void     fftSetAndShiftSlidingInput(FFT* self, fftSample_t* real, fftSample_t* imag /*or NULL*/, int numValidSamples);
void     fftBitReverseIndices      (FFT* self);
void     fftRectangularToPolar     (FFT* self);
void     fftScaleMagnitudes        (FFT* self, fftSample_t min, fftSample_t max); //assumes polar coordinates
unsigned fftSmallestPowerOfTwoGreaterThanOrEqualTo(unsigned n);

/*OpaqueFFTStruct-----------------------------------------------*/
struct OpaqueFFTStruct
{
  unsigned int     numSamples; //may be arbitrary
  unsigned int     N;          //lowest power of 2 greater or equal to numSamples
  unsigned int     overlap;    //numSamples
  fftSample_t*     realSlidingInput;
  fftSample_t*     imagSlidingInput;
  fftSample_t*     real;
  fftSample_t*     imag;
  float*           analysisWindowBuffer;
  fftWindowFunct_t analysisWindowFunct;
};


/*fftNew--------------------------------------------------------*/
FFT* fftNew(int numSamples)
{
  FFT* self = (FFT*) calloc(sizeof(*self), 1);
  if(self != NULL)
    {
      self->numSamples = numSamples;
      self->N = fftSmallestPowerOfTwoGreaterThanOrEqualTo(numSamples);
      self->realSlidingInput      = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->realSlidingInput [0]));
      if(self->realSlidingInput     == NULL) return fftDestroy(self);
      self->imagSlidingInput      = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->imagSlidingInput [0]));
      if(self->imagSlidingInput     == NULL) return fftDestroy(self);
      self->real                  = (fftSample_t*) calloc(self->N, sizeof(self->real  [0]));
      if(self->real                 == NULL) return fftDestroy(self);
      self->imag                  = (fftSample_t*) calloc(self->N, sizeof(self->imag  [0]));
      if(self->imag                 == NULL) return fftDestroy(self);
      self->analysisWindowBuffer  = (float*) calloc(self->N, sizeof(self->analysisWindowBuffer[0]));
      if(self->analysisWindowBuffer == NULL) return fftDestroy(self);
            
      fftSetWindowFunct (self, fftHannWindow);
      fftSetOverlap(self, 3.0/4.0);
    }
  return self;
}

/*fftDestroy----------------------------------------------------*/
FFT* fftDestroy(FFT* self)
{
  if(self != NULL)
    {
      if(self->real != NULL)
        free(self->real);
      if(self->imag != NULL)
        free(self->imag);

      if(self->realSlidingInput != NULL)
        free(self->realSlidingInput);
      if(self->imagSlidingInput != NULL)
        free(self->imagSlidingInput);

      if(self->analysisWindowBuffer != NULL)
        free(self->analysisWindowBuffer);
        
      free(self);
    }
  return (FFT*) NULL;
}

/*-----------------------------------------------------------------------*/
void     fftTransform(FFT* self, fftSample_t*  realIn, fftSample_t*   imagIn, int numInSamples, 
                                 fftSample_t** realOut, fftSample_t**  imagOut, 
                                 BOOL inverse, BOOL bitReversed, BOOL polarOutput)
{
  fftSetAndShiftSlidingInput(self, realIn, imagIn, numInSamples);
  memcpy(self->real, self->realSlidingInput + numInSamples, self->numSamples * sizeof(fftSample_t));
  memcpy(self->imag, self->imagSlidingInput + numInSamples, self->numSamples * sizeof(fftSample_t));
  
  if(!inverse)
    {
      fftApplyWindow(self);
        
      fftRawForwardFFT(self);
      
      if(!bitReversed)
        fftBitReverseIndices(self);
    }
  else
    {
      if(!bitReversed)
        fftBitReverseIndices(self);

      fftRawInverseFFT(self);    
    }
  
  if(polarOutput)
    fftRectangularToPolar(self);  

  if(realOut != NULL)
    *realOut = self->real;
  if(imagOut != NULL)
    *imagOut = self->imag;
}

/*-----------------------------------------------------------------------*/
void     fftSetAndShiftSlidingInput(FFT* self, fftSample_t* real, fftSample_t* imag /*or NULL*/, int numValidSamples)
{
  CONSTRAIN(numValidSamples, 0, self->numSamples);
  
  int n;
  fftSample_t *source, *dest;
  
  n = self->numSamples * sizeof(fftSample_t);
  dest   = self->realSlidingInput;
  source = dest + numValidSamples; // + self->numSamples;
  memmove(dest, source, n); //memcpy(dest, source, n);
  
  n = numValidSamples * sizeof(fftSample_t);
  dest = self->realSlidingInput + self->numSamples; //== source;
  source = real;
  memmove(dest, source, n); //memcpy(dest, source, n);
  n = numValidSamples;
  dest += n;
  while(n++ < self->numSamples)
    *dest++ = 0;

  n = self->numSamples * sizeof(fftSample_t);
  dest   = self->imagSlidingInput;
  source = dest + numValidSamples; // + self->numSamples;
  memmove(dest, source, n); //memcpy(dest, source, n);

  dest = self->imagSlidingInput + self->numSamples; //== source;
  if(imag != NULL)
    {  
      n = numValidSamples * sizeof(fftSample_t);
      source = imag;
      memmove(dest, source, n); //memcpy(dest, source, n);
      n = numValidSamples;
      dest += n;  
    }
  else n = 0;
    
  while(n++ < self->numSamples)
    *dest++ = 0;
}

/*-----------------------------------------------------------------------*/
void   fftFlush                 (FFT* self)
{
  int n = self->numSamples * 2;
  fftSample_t *a, *b;
  //don't worry about the lower half of the buffers, 
  //they will be flushed on the next call to fftSetData()
  
  a = self->realSlidingInput;
  b = self->imagSlidingInput;
  
  while(n-- > 0)
    *a++ = *b++ = 0;
}

/*-----------------------------------------------------------------------*/
void fftSetWindowFunct (FFT* self, fftWindowFunct_t windowFunct)
{
  int i;
  float* buffer           = self->analysisWindowBuffer;
  fftWindowFunct_t*  funct = &(self->analysisWindowFunct);
  *funct = windowFunct;
    
  if(windowFunct != NULL)
    {
      (*funct)(buffer, self->numSamples);
      for(i=self->numSamples; i<self->N; i++)
        buffer[i] = 0;
    }
}

/*-----------------------------------------------------------------------*/
fftWindowFunct_t fftWindowFunction(FFT* self, BOOL analysis)
{
  return self->analysisWindowFunct;
}

/*-----------------------------------------------------------------------*/
void fftRawForwardFFT(FFT* self)
{
  //produces output in bit-reversed order
  int N = self->N;
  fftSample_t *real = self->real, *imag = self->imag;
  //double twoPiOverN = TWO_PI / N, omegaTwoPiOverN;
  uint32_t two_pi_over_N = SIN_TWO_PI / N, omega_two_pi_over_n;
  int sub_transform, butterfly;
  
  int num_sub_transforms = 1, num_butterflies = N/2, omega;
  //int wr, wi, r=this.real, i=this.imag, tempr, tempi;
  fftSample_t wr, wi, *r=real, *i=imag, tempr, tempi;
  int top_index = 0, bottom_index;
  
  while(num_sub_transforms < N)
    {
	  top_index = 0;
      for(sub_transform=0; sub_transform<num_sub_transforms; sub_transform++)
        {
          omega = 0;
          for(butterfly=0; butterfly<num_butterflies; butterfly++)
            {
	          bottom_index        = num_butterflies + top_index;
              omega_two_pi_over_n = omega * two_pi_over_N;
              wr                  =  fastcos(omega_two_pi_over_n);
              wi                  = -fastsin(omega_two_pi_over_n);
              
              tempr               = r[top_index];
              tempi               = i[top_index];
              r[top_index]       += r[bottom_index];
              i[top_index]       += i[bottom_index];
              r[bottom_index]     = tempr - r[bottom_index];
              i[bottom_index]     = tempi - i[bottom_index];
              tempr = (r[bottom_index] * wr) - (i[bottom_index] * wi);
              tempi = (r[bottom_index] * wi) + (i[bottom_index] * wr);
              r[bottom_index]     = tempr;
              i[bottom_index]     = tempi;
              omega              += num_sub_transforms;
              top_index++;
            }
            top_index += num_butterflies;
        }
      num_sub_transforms <<= 1;
      num_butterflies    >>= 1;
    }
}

/*-----------------------------------------------------------------------*/
void fftRawInverseFFT(FFT* self)
{ 
  //takes input in bit-reversed order
  int N = self->N;
  fftSample_t *real = self->real, *imag = self->imag;
  //double twoPiOverN = TWO_PI / N, omegaTwoPiOverN;
  uint32_t two_pi_over_N = SIN_TWO_PI / N, omega_two_pi_over_n;
  int sub_transform, butterfly;
  
  int num_sub_transforms = N, num_butterflies = 1, omega;
  fftSample_t wr, wi, *r=real, *i=imag, tempr, tempi;
  int top_index = 0, bottom_index;
  
  while((num_sub_transforms >>= 1) > 0)
    {
      top_index = 0;
      for(sub_transform=0; sub_transform<num_sub_transforms; sub_transform++)
        {
          omega = 0;
          for(butterfly=0; butterfly<num_butterflies; butterfly++)
            {
	          bottom_index        = num_butterflies + top_index;
              omega_two_pi_over_n = omega * two_pi_over_N;
              wr                  = fastcos(omega_two_pi_over_n);
              wi                  = fastsin(omega_two_pi_over_n);

              tempr = (r[bottom_index] * wr) - (i[bottom_index] * wi);
              tempi = (r[bottom_index] * wi) + (i[bottom_index] * wr);
              wr                  = r[top_index]; 
              wi                  = i[top_index];
              r[top_index]       += tempr;
              i[top_index]       += tempi;
              r[bottom_index]     = wr - tempr;
              i[bottom_index]     = wi - tempi;
              omega              += num_sub_transforms;
              top_index++;
            }
            top_index += num_butterflies;
        }
      num_butterflies <<= 1;
    }
 
  tempr = 1.0/N;
  for(omega=0; omega<N; omega++)
    {
      r[omega] *= tempr;
      i[omega] *= tempr;
    }
}

/*-----------------------------------------------------------------------*/
void fftBitReverseIndices(FFT* self)
{
  fftSample_t *real = self->real, *imag = self->imag;
  int N = self->N;
  int NMinus1 = N-1;
  int highestBit = N >> 1;
  int n, nextBit, nReversed=0;
  fftSample_t temp;
  
  for(n=1; n<N; n++)
    {
      nextBit = highestBit;
      while((nextBit + nReversed) > NMinus1) nextBit >>= 1;  //find highest unpopulated bit
      nReversed &= nextBit - 1;                              //clear all higher bits
      nReversed |= nextBit;                                  //set new bit
      
      if(nReversed > n)
        {
          temp            = real[n]        ;
          real[n]         = real[nReversed];
          real[nReversed] = temp           ;
          temp            = imag[n]        ;
          imag[n]         = imag[nReversed];
          imag[nReversed] = temp           ;          
        }
    }
}

/*-----------------------------------------------------------------------*/
void fftRectangularToPolar(FFT* self)
{
  int i;
  fftSample_t temp;
  unsigned N = self->N;
  fftSample_t* real = self->real;
  fftSample_t* imag = self->imag;
  
  for(i=0; i<N; i++)
    {
      temp = *real;
      *real = hypot(*real, *imag);
      *imag = atan2(*imag, temp);
      real++, imag++;    
    }
}

/*-----------------------------------------------------------------------*/
void fftScaleMagnitudes(FFT* self, fftSample_t min, fftSample_t max)
{
  fftSample_t minSample = 1000000/*FLT_MAX*/, maxSample = 0;
  fftSample_t* r = self->real; 
  unsigned N = self->N;
  
  
  while(N--)
    {
      if(*r < minSample)
        minSample = *r;
      if(*r > maxSample)
        maxSample = *r;
      r++;
    }
    
  N = self->N;
  r = self->real;
  
  while(N-- > 0)
    {
      *r = scalef(*r, minSample, maxSample, min, max);
      r++;
    }
}

/*-----------------------------------------------------------------------*/
void   fftSetOverlap            (FFT* self, float overlap)
{
  unsigned temp = self->numSamples * overlap;
  temp = self->numSamples - temp;
  CONSTRAIN(temp, 1, self->numSamples);
  self->overlap = temp;
}

/*-----------------------------------------------------------------------*/
float  fftOverlap               (FFT* self)
{
  //is this even right?
  return (self->N - self->overlap) / self->numSamples;
}

/*-----------------------------------------------------------------------*/
void fftApplyWindow(FFT* self)
{
  fftWindowFunct_t funct = self->analysisWindowFunct;
  if(funct != NULL)
    {
      fftSample_t *real = self->real, *imag = self->imag;
      float* windowBuffer = self->analysisWindowBuffer;
      unsigned N = self->N;
  
      while(N-- > 0)
        {
          *real++ *= *windowBuffer;
          *imag++ *= *windowBuffer;
          windowBuffer++;
        }
    }
}

/*-----------------------------------------------------------------------*/
/*
void _fftRectWindow(float* windowBuffer, int bufferSize)
{
  int i;
  for(i=0; i<bufferSize; i++)
    *windowBuffer++ = 1;
}
*/
fftWindowFunct_t fftRectWindow = NULL;

/*-----------------------------------------------------------------------*/
void _fftHannWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = 0.5 * (1-cos(phase));
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftHannWindow = _fftHannWindow;

/*-----------------------------------------------------------------------*/
void _fftHammingWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = 0.54 - 0.46 * cos(phase);
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftHammingWindow = _fftHannWindow;

/*-----------------------------------------------------------------------*/
void _fftBlackmanWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  double a = 0;
  double a0 = (1-a)/2.0;
  double a1 = 1/2.0;
  double a2 = a/2.0;
  
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = a0 - a1*cos(phase) + a2*cos(2*phase);
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftBlackmanWindow = _fftBlackmanWindow;

/*-----------------------------------------------------------------------*/
unsigned fftN(FFT* self)
{
  return self->N;
}

/*-----------------------------------------------------------------------*/
unsigned fftNumSamples          (FFT* self)
{
  return self->numSamples;
}

/*-----------------------------------------------------------------------*/
unsigned fftSmallestPowerOfTwoGreaterThanOrEqualTo(unsigned n)
{
  int result = 1;
  while(result < n)
    result <<= 1;
  return result;
}

/*-----------------------------------------------------------------------*/
double   fftFrequencyOfBin         (FFT* self, double bin, double sampleRate)
{
  return bin * (sampleRate / self->N);
}







