#ifndef __FFT__
#define __FFT__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "Util.h"

typedef struct OpaqueFFTStruct FFT;

typedef float fftSample_t;
typedef void (*fftWindowFunct_t)(float*, int);
extern fftWindowFunct_t fftBlackmanWindow, fftRectWindow, fftHannWindow, fftHammingWindow;

FFT*     fftNew                    (int           numSamples); //may be arbitrary
FFT*     fftDestroy                (FFT*          self)             ;

/*
  you can give this as many new samples as you wish, regardless of self->overlap,
  and it will perform an N point DFT with the previous N samples, up-to and
  including the current ones. You may pass NULL for imagIn and / or imagOut
  if you don't care. This function handles windowing. Pass NO for bitReversedOutput
  if you want the data to come in normal, numerical order.
*/
void     fftTransform              (FFT*          self              ,
                                    fftSample_t*  realIn            ,
                                    fftSample_t*  imagIn            ,
                                    int           numInSamples      ,
                                    fftSample_t** realOut           ,
                                    fftSample_t** imagOut           ,
                                    BOOL          inverse           ,
                                    BOOL          bitReversedOutput ,
                                    BOOL          polarOutput      );
double   fftFrequencyOfBin         (FFT*          self, double bin, double sampleRate); //can be in between bins
unsigned fftN                      (FFT*          self);
unsigned fftNumSamples             (FFT*          self);
void     fftSetOverlap             (FFT*          self, float overlap); /* 0 ~ 0.99 */
float    fftOverlap                (FFT*          self);
void     fftFlush                  (FFT*          self);
void     fftSetWindowFunct         (FFT*          self, fftWindowFunct_t windowFunct);
fftWindowFunct_t fftWindowFunction (FFT*          self, BOOL analysis);

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __FFT__
