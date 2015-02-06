/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2013.
 *
 *  mkrzyzan@asu.edu
 */

#include <Arduino.h>
#include <Wire.h>
#include "ColorSensor.h";

/*----------------------------------------------------*/
#define COLOR_SLAVE_ADDR         0x29
#define COLOR_ENABLE_REG_ADDR    0x00
#define COLOR_CONTROL_REG_ADDR   0x0F
#define COLOR_CLEAR_LOW_REG_ADDR COLOR_CLEAR
#define COLOR_RED_LOW_REG_ADDR   RED_CLEAR
#define COLOR_GREEN_LOW_REG_ADDR GREEN_CLEAR
#define COLOR_BLUE_LOW_REG_ADDR  BLUE_CLEAR

/*----------------------------------------------------*/
typedef enum color_enum
{
  COLOR_CLEAR = 0x14,
  COLOR_RED   = 0x16,
  COLOR_GREEN = 0x18,
  COLOR_BLUE  = 0x1A,
}color_t;

float colorGetColor (color_t color);

void     i2c_write_8(uint8_t slave_address, uint8_t  mem_address, uint8_t data);
uint16_t i2c_read_16(uint8_t slave_address, uint16_t mem_address);
uint8_t  i2c_read_8 (uint8_t slave_address, uint8_t  mem_address);

/*----------------------------------------------------*/
void colorInit()
{
  Wire.begin();
  colorSleep();
  delay(3); 
}

/*----------------------------------------------------*/
float colorGetColor (color_t color)
{ 
  unsigned raw = i2c_read_16(COLOR_SLAVE_ADDR, color);
  return (float)raw / (float)0xFFFF;
}

/*----------------------------------------------------*/
void colorGetColorsRGBC(float *r, float *g, float *b, float *c)
{
  *r = colorGetColor (COLOR_RED  );
  *g = colorGetColor (COLOR_GREEN);
  *b = colorGetColor (COLOR_BLUE );
  *c = colorGetColor (COLOR_CLEAR);
/*
  *r *= 1.14286;
  *g *= 1.53846;
  *b *= 1.78571;
*/
}

/*----------------------------------------------------*/
void colorGetColorsHSB (float *h, float *s, float *br)
{
  float r, g, b, c;
  colorGetColorsRGBC(&r, &g, &b, &c);
  colorRGBCtoHSB(r, g, b, c, h, s, br);
}

/*----------------------------------------------------*/
float colorGetLux()
{
  float r, g, b, c;
  colorGetColorsRGBC(&r, &g, &b, &c);
  return colorRGBCtoLux(r, g, b, c);
}

#define colorConstrain(val, low, high) val = (val < low) ? low : (val > high) ? high : val;

/*----------------------------------------------------*/
void  colorRGBCtoHSB   (float r, float g, float b, float c, float *h, float *s, float *br)
{
  
  colorConstrain(r, 0.0, 1.0);
  colorConstrain(g, 0.0, 1.0);
  colorConstrain(b, 0.0, 1.0);
  colorConstrain(c, 0.0, 1.0);
  
  float *hi, *mid, *low;
  
  hi  = (r > g) ? &r : &g;
  if(b > *hi) hi  = &b;
  
  low = (r <= g) ? &r : &g;
  if(b <= *low) low = &b;  
  
  /* brightness */
  *br = *hi;
  if(*br > 0)
    {
      r /= *br;
      g /= *br;
      b /= *br;
    }
  else return; //black -- hue and sat are undefined;
  
  /* saturation */
  *s = 1 - *low;
  
  if(*s > 0)
    {
      r = ((r - 1) / *s) + 1;
      g = ((g - 1) / *s) + 1;
      b = ((b - 1) / *s) + 1;
    }
  else return; //white -- hue is undefined;
  
  /* hue */
  if     ((hi == &r) && (low == &b))
    *h = g  + 0.0;
  else if((hi == &g) && (low == &b))
    *h = 2.0 - r;
  else if((hi == &g) && (low == &r))
    *h = b   + 2.0;
  else if((hi == &b) && (low == &r))
    *h = 4.0 - g;
  else if((hi == &b) && (low == &g))
    *h = r  + 4.0;
  else if((hi == &r) && (low == &g))
    *h = 6.0 - b;
      
  *h /= 6.0;
  
  //else something fucked up;
}

/*----------------------------------------------------*/
void  colorHSBtoRGBC   (float h, float s, float br, float *r, float *g, float *b, float *c)
{  
  while(h > 1)  h--;
  while(h < -1) h++;
  colorConstrain(s , 0, 1);
  colorConstrain(br, 0, 1);
  
  /* hue */
  h *= 6;
  
  if(h < 1.0)
    {
      *r = 1;
      *g = h;
      *b = 0;
    }
  else if(h < 2.0)
    {
      *r = 2.0 - h;
      *g = 1;
      *b = 0;    
    }
  else if(h < 3.0)
    {
      *r = 0;
      *g = 1;
      *b = h - 2.0; 
    }
  else if(h < 4.0)
    {
      *r = 0;
      *g = 4.0 - h;
      *b = 1;
    }
  else if(h < 5.0)
    {
      *r = h - 4.0;
      *g = 0;
      *b = 1;    
    }
  else if(h <= 6.0)
    {
      *r = 1;
      *g = 0;
      *b = 6.0 - h;
    }
  //else something is fucked
  
  /* saturation */
  *r = 1 - (s * (1 - *r));
  *g = 1 - (s * (1 - *g));
  *b = 1 - (s * (1 - *b));
  
  /* brightness */
  *r *= br;
  *g *= br; 
  *b *= br;
  
  *c = *r + *g + *b;
}

/*----------------------------------------------------*/
float colorRGBCtoLux    (float r, float g, float b, float c)
{
  return (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b);
}

/*----------------------------------------------------*/
void colorSetGain(color_gain_t gain)
{
  i2c_write_8(COLOR_SLAVE_ADDR, COLOR_CONTROL_REG_ADDR, gain);
}

/*----------------------------------------------------*/
void colorSleep()
{
  // always keep PON (bit 0) = 1; //2.4 ms warmup delay when set to 1
  // always keep WEN (bit 3) = 0; 
  // toggle AEN (bit 1) to save power
  // all others 0
  i2c_write_8(COLOR_SLAVE_ADDR, COLOR_ENABLE_REG_ADDR, 0x01);  
}

/*----------------------------------------------------*/
void colorWake ()
{
  i2c_write_8(COLOR_SLAVE_ADDR, COLOR_ENABLE_REG_ADDR, 0x03);
  /*
  #define TCS34725_ID               (0x12)
  uint8_t x = i2c_read_8(COLOR_SLAVE_ADDR, TCS34725_ID);
  Serial.print("id: ");
  Serial.println(x, HEX);
  */
}

/*----------------------------------------------------*/
void i2c_write_8(uint8_t slave_address, uint8_t mem_address, uint8_t data)
{
  Wire.beginTransmission(slave_address);
  Wire.write            (0x80 | mem_address  );
  Wire.write            (data         );
  Wire.endTransmission  (             );
}

/*----------------------------------------------------*/
uint8_t i2c_read_8(uint8_t slave_address, uint8_t mem_address)
{
  Wire.beginTransmission(slave_address);
  Wire.write(0x80 | mem_address);
  Wire.endTransmission();

  Wire.requestFrom((int)slave_address, (int)1);
  return Wire.read();
}

/*----------------------------------------------------*/

#define MAKEWORD(H, L) (  ((uint8_t)(H)<<8) | (uint8_t)(L)  )

uint16_t i2c_read_16(uint8_t slave_address, uint16_t mem_address)
{
    uint16_t lsb = 0;
    uint16_t msb = 0;
    int32_t timeout = 160000; //10 ms
    Wire.beginTransmission(slave_address);
    Wire.write            (0x80 | mem_address  );
    Wire.endTransmission  (             );
    Wire.requestFrom((int)slave_address, (int)2);
    while((Wire.available() < 2) && (--timeout > 0));
    if(timeout > 0)
      {
        lsb = Wire.read();
        msb = Wire.read();
      }
    return MAKEWORD(msb, lsb);
}
