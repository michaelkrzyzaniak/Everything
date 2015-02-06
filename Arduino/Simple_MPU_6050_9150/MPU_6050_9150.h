/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2013.
 *
 *  mkrzyzan@asu.edu
 */

#include <stdint.h>

typedef struct opaque_mpu_device_struct MPU;
extern MPU* MPU_1;
extern MPU* MPU_2;

/*----------------------------------------------------*/
void mpu_setup     (MPU* self);

/*----------------------------------------------------*/
void mpu_sleep(MPU* self);
void mpu_wake (MPU* self);

/*----------------------------------------------------*/
void mpu_get_gyro (MPU* self, float *x, float *y, float *z);
void mpu_get_accel(MPU* self, float *x, float *y, float *z);
void mpu_get_mag  (MPU* self, float *x, float *y, float *z);
void mpu_get_temp (MPU* self, float *t);

/*----------------------------------------------------*/
#define MPU_ACCEL_RESOLUTION_2_G  0x00
#define MPU_ACCEL_RESOLUTION_4_G  0x08
#define MPU_ACCEL_RESOLUTION_8_G  0x10
#define MPU_ACCEL_RESOLUTION_16_G 0x18

uint8_t mpu_get_accel_resolution  (MPU* self);
void    mpu_set_accel_resolution  (MPU* self, uint8_t resolution);

/*----------------------------------------------------*/
/* degrees per second */
#define MPU_GYRO_RESOLUTION_250   0x00
#define MPU_GYRO_RESOLUTION_500   0x08
#define MPU_GYRO_RESOLUTION_1000  0x10
#define MPU_GYRO_RESOLUTION_2000  0x18

uint8_t mpu_get_gyro_resolution   (MPU* self);
void    mpu_set_gyro_resolution   (MPU* self, uint8_t resolution);

