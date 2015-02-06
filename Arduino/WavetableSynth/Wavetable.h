/*!
 * @header Wavetable
 * @abstract 
 *
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Summer of 2013.
 *
 *  mkrzyzan@asu.edu
 *
 * @unsorted
 */
/*-----------------------------------------------------*/
#ifndef  __MK_WAVE_TABLE__     
#define  __MK_WAVE_TABLE__ 1 

#ifdef __cplusplus            
extern "C" {                
#endif   

/*-----------------------------------------------------*/
#include "defines.h"
#include <stdint.h>
#include <avr/pgmspace.h>

/*-----------------------------------------------------*/
typedef uint32_t   wavAngle_t;
#define WAV_TWO_PI 0xFFFFFFFF
#define WAV_TABLE_NUM_SAMPLES 2048


/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
uint16_t fastWav  (wavAngle_t angle);
#define fastWav(angle) pgm_read_word_near(wavTable + ((angle) >> 21))

/*-----------------------------------------------------*/
extern prog_uint16_t __attribute__((progmem)) wavTable[WAV_TABLE_NUM_SAMPLES];

#ifdef __cplusplus            
}                             
#endif

#endif//__MK_WAVE_TABLE__     
