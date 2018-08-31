/************************************************
 *	This file and all its contents are	*
 *	licensed under the MIT license.	*
 *	See LICENSE in the root folder for	*
 *	more information.			*
 ************************************************/

/**
 * template.c
 *
 * Created: 2018-07-31
 * Last Modified: 2018-08-21
 * Author(s): Thao Nguyen, Teemu Vierros
 **/


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

#include "sensors.h"	// sensor_t
#include "packing.h"	// to use htno functions

//#include "bma2x2_support.h"
//#include "bmg160_support.h"

#include "bma2x2.h"
#include "bmg160.h"
//#define BMG160_API
//#define BMA2x2_API
#define C_BMG160_TWO_U8X	2
#include "bma2x2_support.c"
#include "bmg160_support.c"



struct bmg160_t bmg160;
struct bmg160_data_t data_gyro;
struct bma2x2_t bma2x2;
struct bma2x2_accel_data sample_xyz;

extern sensor_t bmi055;

extern volatile type state;

s8 I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMG160_INIT_VALUE; //always success
	user_i2c_write( dev_addr, reg_addr, reg_data, cnt);
	return (s8)iError;
}
s8 I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMG160_INIT_VALUE; //always success
	user_i2c_read(dev_addr, reg_addr, reg_data, cnt);
	return (s8)iError;
}
void delay_msek(u32 msek)
{
	nrf_delay_ms(msek);
}
void bmi055_user_init(void)
{

			bmg160.bus_write = I2C_bus_write;
			bmg160.bus_read = I2C_bus_read;
			bmg160.delay_msec = delay_msek;
			bmg160.dev_addr = BMG160_I2C_ADDR1;

			if(bmg160_init(&bmg160)!=0){
				NRF_LOG_WARNING("BMI055(BMG160) initialization failed!");
			}else{
				bmg160_set_power_mode(BMG160_MODE_NORMAL);
				bmg160_set_bw(0x01);
				NRF_LOG_INFO("BMI055(BMG160) initialized!");
			}
		    NRF_LOG_FLUSH();

			bma2x2.bus_write = I2C_bus_write;
			bma2x2.bus_read = I2C_bus_read;
			bma2x2.delay_msec = delay_msek;
			bma2x2.dev_addr = BMA2x2_I2C_ADDR1;

			if(bma2x2_init(&bma2x2)!=0){
				NRF_LOG_WARNING("BMI055(BMA2x2) initialization failed!");
			}else{
				bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);
				bma2x2_set_bw(0x08);
				NRF_LOG_INFO("BMI055(BMA2x2) initialized!");
			}
			NRF_LOG_FLUSH();
}

void bmi055_user_wake(void){
	bma2x2_set_power_mode(0);
	//nrf_delay_ms(30);
	//nrf_delay_ms(225);
}
void bmi055_user_sleep(void){
	bma2x2_set_power_mode(2);
	//nrf_delay_ms(30);
	//nrf_delay_ms(225);
}


void bmi055_user_read(void)
{
	//read GYRO

	uint8_t * pos = (uint8_t *) bmi055.p_data;
#define LONG	1
#if LONG
	uint16_t gyro[3] = {0};
	if(state == BMI_GYRO_TEMP){
		bmg160_get_data_XYZ(&data_gyro);

		gyro[0] = htno_16((uint16_t)data_gyro.datax);
		gyro[1] = htno_16((uint16_t)data_gyro.datay);
		gyro[2] = htno_16((uint16_t)data_gyro.dataz);

		NRF_LOG_INFO("bmi055 gyrox %d (0x%x)",(int16_t)data_gyro.datax,(int16_t)data_gyro.datax);
		NRF_LOG_INFO("bmi055 gyroy %d (0x%x)",(int16_t)data_gyro.datay,(int16_t)data_gyro.datay);
		NRF_LOG_INFO("bmi055 gyroz %d (0x%x)",(int16_t)data_gyro.dataz,(int16_t)data_gyro.dataz);
	}else{
		gyro[0] = 0;
		gyro[1] = 0;
		gyro[2] = 0;
	}
#else
	uint8_t gyro[3] = {0};
	if(state == BMI_GYRO_TEMP){
		bmg160_get_data_XYZ(&data_gyro);
		gyro[0] = (uint8_t)htno_16((uint16_t)data_gyro.datax);
		gyro[1] = (uint8_t)htno_16((uint16_t)data_gyro.datay);
		gyro[2] = (uint8_t)htno_16((uint16_t)data_gyro.dataz);
		NRF_LOG_INFO("bmi055 gyro: x = %d (0x%x), y = %d (0x%x), z = %d (0x%x)",(int8_t)gyro[0],gyro[0],(int8_t)gyro[1],gyro[1],(int8_t)gyro[2],gyro[2]);
	}else{
		gyro[0] = 0;
		gyro[1] = 0;
		gyro[2] = 0;
	}
#endif

	memcpy(pos,&gyro,sizeof(gyro));
	pos += sizeof(gyro);

	//read ACCELERATION

#if LONG
	uint16_t accel[3] = {0};
	if(state == BMI_ACCEL_APDS_PG){
		bma2x2_read_accel_xyz(&sample_xyz);
		accel[0] = htno_16((uint16_t)sample_xyz.x);
		accel[1] = htno_16((uint16_t)sample_xyz.y);
		accel[2] = htno_16((uint16_t)sample_xyz.z);

		NRF_LOG_INFO("bmi055 accelx %d (0x%x)",(int16_t)sample_xyz.x,(int16_t)sample_xyz.x);
		NRF_LOG_INFO("bmi055 accely %d (0x%x)",(int16_t)sample_xyz.y,(int16_t)sample_xyz.y);
		NRF_LOG_INFO("bmi055 accelz %d (0x%x)",(int16_t)sample_xyz.z,(int16_t)sample_xyz.z);
	}else{
		accel[0] = 0;
		accel[1] = 0;
		accel[2] = 0;
	}
#else
	uint8_t accel[3] = {0};
	if(state == BMI_ACCEL_APDS_PG){
		bma2x2_read_accel_xyz(&sample_xyz);
		accel[0] = (uint8_t)htno_16((uint16_t)sample_xyz.x);
		accel[1] = (uint8_t)htno_16((uint16_t)sample_xyz.y);
		accel[2] = (uint8_t)htno_16((uint16_t)sample_xyz.z);
		NRF_LOG_INFO("bmi055 accel: x = %d (0x%x), y = %d (0x%x), z = %d (0x%x)",(int8_t)accel[0],accel[0],(int8_t)accel[1],accel[1],(int8_t)accel[2],accel[2]);
	}else{
		accel[0] = 0;
		accel[1] = 0;
		accel[2] = 0;
	}
	#endif

	memcpy(pos,&accel,sizeof(accel));
	pos += sizeof(accel);

}


