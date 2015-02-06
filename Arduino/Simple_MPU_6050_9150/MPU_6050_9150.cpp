/*!
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Fall of 2013.
 *
 *  mkrzyzan@asu.edu
 */

#include "../../../../libraries/Wire/Wire.h"
#include "MPU_6050_9150.h"

#define MSB(z)         (  (uint8_t)(z>>8)  )
#define LSB(z)         (  (uint8_t)(z&0x00FF)  )
#define MAKEWORD(H, L) (  ((uint8_t)(H)<<8) | (uint8_t)(L)  )
#define BIG_ENDIAN    0
#define LITTLE_ENDIAN 1
#define PI 3.141592653589793


/* if ADC0 pin is connected to ground */
#define MPU_1_SLAVE_ADDRESS   0x68
/* if ADC0 pin is connected to VCC */
#define MPU_2_SLAVE_ADDRESS   0x69
/* put MPU into pass through mode before using this */
#define MPU_MAG_SLAVE_ADDRESS 0x0C 

//#define MPU_USER_CTRL_ADDRESS 0x6A //clear bit 5 to read mag

/* degrees per second */
#define MPU_GYRO_CONFIG_ADDRESS    0x1B
#define MPU_GYRO_SENSITIVITY_250   0x00
#define MPU_GYRO_SENSITIVITY_500   0x08
#define MPU_GYRO_SENSITIVITY_1000  0x10
#define MPU_GYRO_SENSITIVITY_2000  0x18

#define MPU_ACCEL_CONFIG_ADDRESS   0x1C
#define MPU_ACCEL_SENSITIVITY_2_G  0x00
#define MPU_ACCEL_SENSITIVITY_4_G  0x08
#define MPU_ACCEL_SENSITIVITY_8_G  0x10
#define MPU_ACCEL_SENSITIVITY_16_G 0x18

#define MPU_TEMP_ADDRESS           0x41

#define MPU_ACCEL_X_ADDRESS        0x3B
#define MPU_ACCEL_Y_ADDRESS        0x3D
#define MPU_ACCEL_Z_ADDRESS        0x3F

#define MPU_GYRO_X_ADDRESS         0x43
#define MPU_GYRO_Y_ADDRESS         0x45
#define MPU_GYRO_Z_ADDRESS         0x47

/* the first 6 EXT_SENS_DATA registers of MPU9150 */
#define MPU_MAG_X_ADDRESS          0x49
#define MPU_MAG_Y_ADDRESS          0x51
#define MPU_MAG_Z_ADDRESS          0x53

#define MPU_MAG_X_NATIVE_ADDRESS   0x03
#define MPU_MAG_Y_NATIVE_ADDRESS   0x05
#define MPU_MAG_Z_NATIVE_ADDRESS   0x07

/* for sleep and wake */
#define MPU_POWER_ADDRESS          0x6B

/* for direcly accessing mag */
#define MPU_USER_CTRL_ADDRESS      0x6A /* write bit 5 low for direct access*/
#define MPU_BYPASS_ADDRESS         0x37 /* write bit 1 high for direct access*/

/* for configuring 'external' sensor (i.e. mag) */
#define MPU_I2C_MST_CTRL_ADDRESS   0x24
#define MPU_SLV0_ADDR_ADDRESS      0x25
#define MPU_SLV0_REG_ADDRESS       0x26
#define MPU_SLV0_CTRL_ADDRESS      0x27
#define MPU_SLV1_ADDR_ADDRESS      0x28
#define MPU_SLV1_REG_ADDRESS       0x29
#define MPU_SLV1_CTRL_ADDRESS      0x2A
#define MPU_SLV1_DATA_OUT_ADDRESS  0x64
/*----------------------------------------------------*/
struct opaque_mpu_device_struct
{
  uint8_t slave_address;
  float   accel_scaling;
  float   gyro_scaling;
};

MPU MPU_1_STATIC;
MPU MPU_2_STATIC;

MPU* MPU_1 = &MPU_1_STATIC;
MPU* MPU_2 = &MPU_2_STATIC;

/*----------------------------------------------------*/
void     i2c_write_8    (uint8_t slave_address, uint8_t  mem_address, uint8_t data);
uint8_t  i2c_read_8     (uint8_t slave_address, uint8_t  mem_address);
void     i2c_write_16   (uint8_t slave_address, uint16_t mem_address, uint16_t data, int endian);
uint16_t i2c_read_16    (uint8_t slave_address, uint16_t mem_address, int endian);
void     i2c_read_buffer(uint8_t slave_address, uint16_t mem_address, char* buffer, int num_bytes);

void     mpu_set_resolution (uint8_t slave_address, uint8_t  registerAddress, uint8_t sensitivity);
uint8_t  mpu_get_resolution (uint8_t slave_address, uint8_t registerAddress);
  void mpu_setup_mag(MPU* self);
/*----------------------------------------------------*/
void i2c_write_8(uint8_t slave_address, uint8_t mem_address, uint8_t data)
{
  Wire.beginTransmission(slave_address);
  Wire.write            (mem_address  );
  Wire.write            (data         );
  Wire.endTransmission  (             );
}

/*----------------------------------------------------*/
uint8_t i2c_read_8(uint8_t slave_address, uint8_t mem_address)
{
    unsigned int timeout = 16000;
    Wire.beginTransmission(slave_address);
    Wire.write            (mem_address  );
    Wire.endTransmission  (             );
    Wire.requestFrom((int)slave_address, (int)1);
    while((Wire.available() < 1) && (timeout-- > 0));
    uint8_t data = Wire.read();
    return data;
}

/*----------------------------------------------------*/
void i2c_write_16(uint8_t slave_address, uint16_t mem_address, uint16_t data, int endian)
{
  Wire.beginTransmission(slave_address);
  Wire.write(mem_address);
  if(endian == LITTLE_ENDIAN)
    {
      Wire.write(MSB(data));
      Wire.write(LSB(data));
    }
  else
    {
      Wire.write(LSB(data));
      Wire.write(MSB(data));
    }
  Wire.endTransmission();
}

/*----------------------------------------------------*/
uint16_t i2c_read_16(uint8_t slave_address, uint16_t mem_address, int endian)
{
    unsigned int timeout = 16000;
    Wire.beginTransmission(slave_address);
    Wire.write            (mem_address  );
    Wire.endTransmission  (             );
    Wire.requestFrom((int)slave_address, (int)2);
    while((Wire.available() < 2) && (timeout-- > 0));
    uint16_t msb = Wire.read();
    uint16_t lsb = Wire.read();
    if(endian == BIG_ENDIAN)
      return MAKEWORD(msb, lsb);
    else
      return MAKEWORD(lsb, msb);
}

/*----------------------------------------------------*/
void i2c_read_buffer(uint8_t slave_address, uint16_t mem_address, uint8_t* buffer, int num_bytes)
{
    unsigned int timeout = 16000;
    Wire.beginTransmission(slave_address);
    Wire.write(mem_address);
    Wire.endTransmission();
    Wire.requestFrom((int)slave_address, num_bytes);
    while((Wire.available() < num_bytes) && (timeout-- > 0));
    while(num_bytes-- > 0)
      *buffer++ = Wire.read();
}

/*----------------------------------------------------*/
void mpu_setup     (MPU* self)
{
  uint8_t resolution, config;
  Wire.begin();
  self->slave_address = (self == MPU_1) ? MPU_1_SLAVE_ADDRESS : MPU_2_SLAVE_ADDRESS;
  resolution = mpu_get_gyro_resolution(self);
  mpu_set_gyro_resolution(self, resolution);
  resolution = mpu_get_accel_resolution(self);
  mpu_set_accel_resolution(self, resolution);
  
//#ifdef MPU_9150
  mpu_setup_mag(self);
 //#endif //mpu9150

  mpu_wake(self);
}

/*----------------------------------------------------*/
void mpu_sleep(MPU* self)
{
  int value = i2c_read_8(self->slave_address, MPU_POWER_ADDRESS);
  value |= 0x40;
  i2c_write_8(self->slave_address, MPU_POWER_ADDRESS, value);
}

/*----------------------------------------------------*/
void mpu_wake(MPU* self)
{
  int value = i2c_read_8(self->slave_address, MPU_POWER_ADDRESS);
  value &= 0xBF; //wake
  value |= 0x01; //use gyro as clock
  i2c_write_8(self->slave_address, MPU_POWER_ADDRESS, value);
}

/*----------------------------------------------------*/
void mpu_get_gyro (MPU* self, float *x, float *y, float *z)
{
  uint8_t buffer[6];
  int16_t ix, iy, iz;
  i2c_read_buffer(self->slave_address, MPU_GYRO_X_ADDRESS, buffer, 6);
  ix = MAKEWORD(buffer[0], buffer[1]);
  iy = MAKEWORD(buffer[2], buffer[3]);
  iz = MAKEWORD(buffer[4], buffer[5]); 
  *x = ix / self->gyro_scaling;
  *y = iy / self->gyro_scaling;
  *z = iz / self->gyro_scaling;
}

/*----------------------------------------------------*/
void mpu_get_accel(MPU* self, float *x, float *y, float *z)
{
  uint8_t buffer[6];
  int16_t ix, iy, iz;
  i2c_read_buffer(self->slave_address, MPU_ACCEL_X_ADDRESS, buffer, 6);
  ix = MAKEWORD(buffer[0], buffer[1]);
  iy = MAKEWORD(buffer[2], buffer[3]);
  iz = MAKEWORD(buffer[4], buffer[5]); 
  *x = ix / self->accel_scaling;
  *y = iy / self->accel_scaling;
  *z = iz / self->accel_scaling;
}

/*----------------------------------------------------*/
void mpu_get_mag  (MPU* self, float *x, float *y, float *z)
{
  uint8_t buffer[6];
  int16_t ix, iy, iz;

  i2c_read_buffer(self->slave_address, MPU_MAG_X_ADDRESS, buffer, 6);
  ix = MAKEWORD(buffer[0], buffer[1]);
  iy = MAKEWORD(buffer[2], buffer[3]);
  iz = MAKEWORD(buffer[4], buffer[5]);
 
  *x = PI * ix / 4096.0;
  *y = PI * iy / 4096.0;
  *z = PI * iz / 4096.0;
}

/*----------------------------------------------------*/
void mpu_get_temp (MPU* self, float *temp)
{
  int t = i2c_read_16(self->slave_address, MPU_TEMP_ADDRESS, BIG_ENDIAN);
  //*temp = (t / 340.0) + 35; //Celsius
  *temp = 1.8 * t / 340 + 95; //Farenheit
}

/*----------------------------------------------------*/
void mpu_set_gyro_resolution (MPU* self, uint8_t resolution)
{
  mpu_set_resolution(self->slave_address, MPU_GYRO_CONFIG_ADDRESS, resolution);
  switch(resolution)
    {
      case MPU_GYRO_RESOLUTION_250 : self->gyro_scaling = 131 ; break;
      case MPU_GYRO_RESOLUTION_500 : self->gyro_scaling = 65.5; break;
      case MPU_GYRO_RESOLUTION_1000: self->gyro_scaling = 32.8; break;
      case MPU_GYRO_RESOLUTION_2000: self->gyro_scaling = 16.4; break;
      default: self->gyro_scaling = 1234;
    }
}

/*----------------------------------------------------*/
void mpu_set_accel_resolution (MPU* self, uint8_t resolution)
{
  mpu_set_resolution(self->slave_address, MPU_ACCEL_CONFIG_ADDRESS, resolution);
  switch(resolution)
    {
      case MPU_ACCEL_RESOLUTION_2_G : self->accel_scaling = 16384; break;
      case MPU_ACCEL_RESOLUTION_4_G : self->accel_scaling = 8192 ; break;
      case MPU_ACCEL_RESOLUTION_8_G : self->accel_scaling = 4096 ; break;
      case MPU_ACCEL_RESOLUTION_16_G: self->accel_scaling = 2048 ; break;
      default: break;
    }
}

/*----------------------------------------------------*/
uint8_t  mpu_get_gyro_resolution (MPU* self)
{
  return mpu_get_resolution(self->slave_address, MPU_GYRO_CONFIG_ADDRESS);
}

/*----------------------------------------------------*/
uint8_t  mpu_get_accel_resolution (MPU* self)
{
  return mpu_get_resolution(self->slave_address, MPU_ACCEL_CONFIG_ADDRESS);
}

/*----------------------------------------------------*/
void mpu_set_resolution(uint8_t slave_address, uint8_t register_address, uint8_t resolution)
{
  uint8_t val = mpu_get_resolution(slave_address, register_address);
  val |= resolution; //set sensitivity bits
  i2c_write_8(slave_address, register_address, val); 
}

/*----------------------------------------------------*/
uint8_t mpu_get_resolution(uint8_t slave_address, uint8_t register_address)
{
  uint8_t val;
  val = i2c_read_8(slave_address, register_address);
  val &= 0xE7;
  return val;
}

/*----------------------------------------------------*/
void mpu_setup_mag(MPU* self)
{
  i2c_write_8(MPU_MAG_SLAVE_ADDRESS, 0x0A, 0x00); //PowerDownMode
  i2c_write_8(MPU_MAG_SLAVE_ADDRESS, 0x0A, 0x0F); //SelfTest
  i2c_write_8(MPU_MAG_SLAVE_ADDRESS, 0x0A, 0x00); //PowerDownMode
 
  //i2c_write_8(self->slave_address, MPU_I2C_MST_CTRL_ADDRESS, 0x40);
  i2c_write_8(self->slave_address, MPU_SLV0_ADDR_ADDRESS   , 0x80 | MPU_MAG_SLAVE_ADDRESS);
  i2c_write_8(self->slave_address, MPU_SLV0_REG_ADDRESS    , MPU_MAG_X_NATIVE_ADDRESS);
  i2c_write_8(self->slave_address, MPU_SLV0_CTRL_ADDRESS   , 0xD6);

  /* put "single measuerment mode" in mag's CNTL register" */
  i2c_write_8(self->slave_address, MPU_SLV1_ADDR_ADDRESS, MPU_MAG_SLAVE_ADDRESS);
  i2c_write_8(self->slave_address, MPU_SLV1_REG_ADDRESS , 0x0A);
  i2c_write_8(self->slave_address, MPU_SLV1_CTRL_ADDRESS, 0x81);
  i2c_write_8(self->slave_address, MPU_SLV1_DATA_OUT_ADDRESS, 0x01);
  
  i2c_write_8(self->slave_address, 0x67, 0x03); //set delay rate

  //i2c_write_8(self->slave_address, 0x34, 0x04); //set i2c slv4 delay
  //i2c_write_8(self->slave_address, MPU_SLV1_DATA_OUT_ADDRESS, 0x00); //override register
  //i2c_write_8(self->slave_address, MPU_USER_CTRL_ADDRESS    , 0x00); //clear usr setting
  //i2c_write_8(self->slave_address, MPU_SLV1_DATA_OUT_ADDRESS, 0x01); //override register
  
  i2c_write_8(self->slave_address, MPU_USER_CTRL_ADDRESS   , 0x20); 
  i2c_write_8(self->slave_address, 0x34, 0x13); //disable slv4
}
