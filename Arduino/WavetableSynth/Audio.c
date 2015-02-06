/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Spring of 2013.
 *
 *  mkrzyzan@asu.edu
 *
 *  Modified August 18 2013 to remove floating point math from critical sections
 *
 */

#include "Arduino.h"
#include "Audio.h"
#include "Wavetable.h"

//#define __USE_FLOATING_POINT_MATH__ 1


#define     AU_TWO_PI                          WAV_TWO_PI
const int   AU_SAMPLE_RATE                   = F_CPU / (AU_RESOLUTION + 1); //16000000 / 0x3FF = 0x3d18 (15640)
const float AU_TWO_PI_OVER_SAMPLE_RATE       = 274614.2772;//AU_TWO_PI / AU_SAMPLE_RATE; //4294967295 / 15640 = 274614.2772
const float AU_TWO_PI_OVER_CTRL_RATE         = AU_TWO_PI / AU_CONTROL_RATE;
const float AU_CONTROL_RATE_OVER_1000        = AU_CONTROL_RATE / 1000.0;

volatile wavAngle_t auPhase                  = 0;
volatile int32_t    auActualFreq             = 0;
volatile int32_t    auTargetFreq             = 0;
volatile int32_t    auGlideTime              = 0; /*num control periods*/
volatile int32_t    auFreqGlideTimeRemaining = 0;
volatile int32_t    auVolGlideTimeRemaining  = 0;

#ifdef __USE_FLOATING_POINT_MATH__
volatile float      auFreqGlideIncrement     = 0;
volatile float      auVolGlideIncrement      = 0;
volatile float      auActualVol              = 1;
volatile float      auTargetVol              = 1;
#else
volatile int32_t    auFreqGlideIncrement     = 0;
volatile int16_t    auVolGlideIncrement      = 0;
volatile uint16_t   auActualVol              = 1;
volatile uint16_t   auTargetVol              = 1;
#endif

/*-----------------------------------------------------*/
void auStart()
{
 //sample clock
  OCR1A   = AU_RESOLUTION/2;
  OCR1B   = AU_RESOLUTION/2;
  ICR1    =   AU_RESOLUTION; // timer TOP
  sbi( TCCR1A, COM1A1 ); // inverted PWM on pin 9
  sbi( TCCR1A, COM1A0 ); //
  sbi( TCCR1A, COM1B1 ); // non-inverted PWM on 10
  cbi( TCCR1A, COM1B0 ); //
  cbi( TCCR1A, WGM10  ); // fast pwm with TOP = ICR1
  sbi( TCCR1A, WGM11  ); //
  sbi( TCCR1B, WGM12  ); //
  sbi( TCCR1B, WGM13  ); //
  sbi( TCCR1B, CS10   ); // no clock prescaler
  cbi( TCCR1B, CS11   ); //
  cbi( TCCR1B, CS12   ); // 1, 0, 0 (1 0 1 divide by 1024)
  sbi( TIMSK1, TOIE1  ); // enable interrupt on TOP
  sbi( DDRB  , DDB1   ); // pin B1 / OC1A / Arduino 9  is output
  sbi( DDRB  , DDB2   ); // pin B2 / OC1B / Arduino 10 is output
  
  //control clock
  cbi( TCCR2A, WGM20  ); // CTC mode with TOP = OCR2A
  sbi( TCCR2A, WGM21  ); //
  cbi( TCCR2B, WGM22  ); //
  sbi( TCCR2B, CS20   ); // 1024 prescaler
  sbi( TCCR2B, CS21   ); // 
  sbi( TCCR2B, CS22   ); // 
  sbi( TIMSK2, OCIE2A ); // output compare match interrupt
  cbi( ASSR  , AS2    ); // use system clock (not oscillator);
  OCR2A = F_CPU / (1024L * AU_CONTROL_RATE) - 1; //timer TOP
  
  cbi( PRR   , PRTIM1 ); // take timer 1 out of power-saving mode
  cbi( PRR   , PRTIM2 ); // take timer 2 out of power-saving mode
}

/*-----------------------------------------------------*/
ISR(TIMER1_OVF_vect)
{
#if defined __USE_FLOATING_POINT_MATH__
  OCR1A = OCR1B = ((fastWav(auPhase))) * auActualVol;
#else
  uint32_t temp = fastWav(auPhase);
  temp *= auActualVol;
  temp >>= 16;
  OCR1A = OCR1B = temp;
#endif

  auPhase += auActualFreq;
}
/*
#ifdef __USE_FLOATING_POINT_MATH__
#else
#endif
*/
/*-----------------------------------------------------*/
ISR(TIMER2_COMPA_vect)
{
  if(auFreqGlideTimeRemaining > 0)
    {
      auFreqGlideTimeRemaining--;
      auActualFreq += auFreqGlideIncrement;
    }
   else 
     auActualFreq = auTargetFreq;
 
  if(auVolGlideTimeRemaining > 0)
    {
      auVolGlideTimeRemaining--;
      auActualVol += auVolGlideIncrement;
    }
  else 
     auActualVol = auTargetVol;
}

/*-----------------------------------------------------*/
void auStop()
{
  //this function needs serious help!
  TIMSK1 &=  ~( 1<<TOIE1 ); //disable interrupt on TOP
}

/*-----------------------------------------------------*/
void   auSetFreq(float freq)
{
  cbi( TIMSK2, OCIE2A );
  auTargetFreq             = freq * AU_TWO_PI_OVER_SAMPLE_RATE;
#ifdef __USE_FLOATING_POINT_MATH__
  auFreqGlideIncrement     = (auTargetFreq - auActualFreq) / auGlideTime;
#else
  auFreqGlideIncrement     = ((int32_t)auTargetFreq - (int32_t)auActualFreq) / auGlideTime;
#endif

  auFreqGlideTimeRemaining = auGlideTime;
  sbi( TIMSK2, OCIE2A );
}

/*-----------------------------------------------------*/
float  auGetFreq   ()
{
  return auTargetFreq / AU_TWO_PI_OVER_SAMPLE_RATE;
}


/*-----------------------------------------------------*/
void auSetGlideTime(int milliseconds)
{
  if(milliseconds < 0) milliseconds = 0;
  auGlideTime = milliseconds * AU_CONTROL_RATE_OVER_1000 + 1;
}

/*-----------------------------------------------------*/
float  auGetGlideTime   ()
{
  return auGlideTime / AU_CONTROL_RATE_OVER_1000 - 1;
}

/*-----------------------------------------------------*/
void auSetVol(float vol)
{
  //if(vol < 0) vol = 0;
  //if(vol > 1) vol = 1;
  //vol *= vol;
  
  cbi( TIMSK2, OCIE2A );
#if defined __USE_FLOATING_POINT_MATH__
  auTargetVol             = vol;
  auVolGlideIncrement     = (auTargetVol - auActualVol) / auGlideTime;
#else
  auTargetVol = vol * 0xFFFF;
  auVolGlideIncrement     = ((int32_t)auTargetVol - (int32_t)auActualVol) / auGlideTime;
#endif

  auVolGlideTimeRemaining = auGlideTime;
  sbi( TIMSK2, OCIE2A );
}

/*-----------------------------------------------------*/
float auGetVol()
{
  return /*sqrt*/(auTargetVol);
}






