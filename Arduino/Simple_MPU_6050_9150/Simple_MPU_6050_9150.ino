/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2013.
 *
 *  mkrzyzan@asu.edu
 */

#include <stdint.h>
#include "Wire.h"
#include "MPU_6050_9150.h"

/*----------------------------------------------------*/
void setup(void)
{
  Serial.begin(115200);
  delay(5); //let the MPU power up;

  /* use MPU_2 if ADC0 is high, else MPU_1 */
  mpu_setup(MPU_1);
  
  /* set accel sensitivity  (_2_G, _4_G, _8_G, or _16_G) */
  mpu_set_accel_resolution(MPU_1, MPU_ACCEL_RESOLUTION_4_G);

  /* set gyro sensitivity (_250, _500, _1000, or _2000 deg / sec) */
  mpu_set_gyro_resolution (MPU_1, MPU_GYRO_RESOLUTION_250 );
}

/*----------------------------------------------------*/
void loop(void)
{
  float x, y, z;
  
  /* read the accelerometer */
  mpu_get_accel(MPU_1, &x, &y, &z);
  print_xyz("Accel:", x, y, z);
  
  /* read the gyroscope */
  mpu_get_gyro(MPU_1, &x, &y, &z);
  print_xyz("Gyro:", x, y, z);

  /* read the magnetometer */
  mpu_get_mag(MPU_1, &x, &y, &z);
  print_xyz("Mag:", x, y, z);

  /* read the temperature */
  mpu_get_temp(MPU_1, &x);
  Serial.print("Temp: ");
  Serial.println(x);
  
  delay(100);
}

/*----------------------------------------------------*/
void print_xyz(const char* what, float x, float y, float z)
{
  Serial.print(what);
  Serial.print(" ");
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");  
  Serial.print(z);
  Serial.print("\r\n");
}

