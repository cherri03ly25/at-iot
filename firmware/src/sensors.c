/************************************************
 *	This file and all its contents are	*
 *	licensed under the MIT license.	*
 *	See LICENSE in the root folder for	*
 *	more information.			*
 ************************************************/

/**
 * template.c
 *
 * Created: 2018-07-25
 * Last Modified: 2018-08-21
 * Author(s): Thao Nguyen, Teemu Vierros
 **/

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "sensors.h"
#include "packing.h"
#include "bme280_user_def.h"
#include "bmi055_user_def.h"
#include "apds9960_user_def.h"


uint8_t temp_data[TEMP_LEN] = {0};
sensor_t temp = {temp_data,TEMP_LEN,internal_temp_init,get_internal_temp};

uint8_t bme280_data[BME280_LEN] = {0};
sensor_t bme280 = {bme280_data,BME280_LEN,bme280_user_init,bme280_user_read};

uint8_t bmi055_data[BMI055_LEN] = {0};
sensor_t bmi055 = {bmi055_data,BMI055_LEN,bmi055_user_init,bmi055_user_read, bmi055_user_wake, bmi055_user_sleep};

uint8_t apds9960_data[APDS9960_LEN] = {0};
sensor_t apds9960 = {apds9960_data, APDS9960_LEN, apds9960_user_init, apds9960_user_read, apds9960_user_wake, apds9960_user_sleep};

volatile type state = 0;

/**@brief Function for reading internal temperature of nrf52 SoC.
 */
void get_internal_temp(void)
{
    int32_t rst;
    uint32_t err_code;

    err_code = sd_temp_get(&rst);
    APP_ERROR_CHECK(err_code);

    rst = (rst*100)/4 ;
    NRF_LOG_INFO("internal temp %d (0x%x)", rst, rst);

    rst = htno_16((uint16_t)rst);

	uint8_t * pos = (uint8_t *) temp.p_data;
	memcpy(pos, &rst , 2);
}
/**@brief Function for reading internal temperature of nrf52 SoC.
 */
void internal_temp_init(void)
{
	NRF_LOG_INFO("TEMP initialized!");
    NRF_LOG_FLUSH();
}


void sensors_init(void)
{
    twi_init();
	temp.init();

	bme280.init();
	//bme280.sleep();

	bmi055.init();
	bmi055.sleep();

	apds9960.init();
	apds9960.sleep();
}


#if !STOP
void sensors_read(void)
{
    switch(state){
		case BME_THP:
			bme280.read();
			break;
		case BMI_GYRO_TEMP:
			temp.read();
			bmi055.wake();
			bmi055.read();
			//bmi055.sleep();
			//nrf_delay_ms(255);
			break;
		case BMI_ACCEL_APDS_PG:
			//bmi055.wake();
			bmi055.read();
			bmi055.sleep();
			apds9960.wake();
			apds9960.read();
			//apds9960.sleep();
			break;
		case APDS_COLOR:
			//apds9960.wake();
			apds9960.read();
			apds9960.sleep();
			break;
		default:
			break;
    }
}
#endif






