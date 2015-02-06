/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2013.
 *
 *  mkrzyzan@asu.edu
 */


typedef enum color_gain_enum
{
  COLOR_GAIN_1X  = 0x00,
  COLOR_GAIN_4X  = 0x01,
  COLOR_GAIN_16X = 0x02,
  COLOR_GAIN_60X = 0x03,
}color_gain_t;

void  colorInit ();
void  colorSleep();
void  colorWake ();

void  colorGetColorsRGBC(float *r, float *g, float *b, float *c);
void  colorGetColorsHSB (float *h, float *s, float *br);
void  colorRGBCtoHSB    (float r, float g, float b, float c, float *h, float *s, float *br);
void  colorHSBtoRGBC    (float h, float s, float l, float *r, float *g, float *b, float *c);
float colorGetLux       ();
float colorRGBCtoLux    (float r, float g, float b, float c);
void  colorSetGain      (color_gain_t gain);

