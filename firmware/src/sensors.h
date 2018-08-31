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


//https://stackoverflow.com/questions/1350376/function-pointer-as-a-member-of-a-c-struct/46140014

#ifndef DRV_Sensors_H_
#define DRV_Sensors_H_

#include "i2c.h"

#define BME280_LEN		(8U)
#define BMI055_LEN		(12U)
#define APDS9960_LEN	(10U)
#define TEMP_LEN		(2U)

#define BME_TEMP_HUM			0x00
#define BME_PRESS				0x01
#define APDS_COLOR1				0x02
#define APDS_COLOR2				0x03
#define BMI_GYROXY				0x04
#define BMI_GYROZ_TEMP			0x05
#define BMI_ACCELXY				0x06
#define BMI_ACCELZ_ADPS_PG		0x07
/*
typedef enum {
	TEMP,
	BME280,
	BMI055,
	APDS9960,
	BMI055_GYRO,
	BMI055_ACCEL
} type;
*/

typedef enum {
	BME_THP,
	BMI_GYRO_TEMP,
	BMI_ACCEL_APDS_PG,
	APDS_COLOR,
	NUMBER_OF_STATES
} type;

typedef struct {
	uint8_t * p_data;
	size_t data_len;
	void (*init)(void);
	void (*read)(void);
	void (*wake)(void);
	void (*sleep)(void);
} sensor_t;




//void init(void * self);

//void read(void * self);

void sensors_init(void);

void sensors_read(void);


/**@brief Function for reading internal temperature of nrf52 SoC.
 */
void get_internal_temp(void);

void internal_temp_init(void);


#endif /* DRV_Sensors_H_ */
