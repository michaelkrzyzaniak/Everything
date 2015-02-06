/*!
 * @header Audio
 * @abstract synthesize a sine-wave with differential
 *   output on pins 9 and 10;
 *
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Spring of 2013.
 *
 *  mkrzyzan@asu.edu
 *
 * @unsorted
 */
/*-----------------------------------------------------*/

#ifndef  __MK_AUDIO__     
#define  __MK_AUDIO__ 1 

#ifdef __cplusplus            
extern "C" {                
#endif 

/*-----------------------------------------------------*/
#define     AU_CONTROL_RATE              1000    // 31 ~ 7813
#define     AU_RESOLUTION                0x3FF   //10 bits, freq = (16000000 / 1+res)  

#ifdef MEGAPHONE
extern unsigned short auAudioSample;
#endif

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
void auStart();

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
void auStop();

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
void   auSetFreq(float freq);

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
float  auGetFreq();

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
void  auSetGlideTime(int milliseconds);

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
float auGetGlideTime   ();

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
void  auSetVol(float vol);

/*-----------------------------------------------------*/
/*! 
 * @function
 * @abstract 
 */
float auGetVol   ();

#ifdef __cplusplus            
}                             
#endif

#endif//__MK_AUDIO__
