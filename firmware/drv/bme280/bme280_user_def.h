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


#ifndef DRV_BME280_BME280_USER_DEF_H_
#define DRV_BME280_BME280_USER_DEF_H_

#include "bme280_defs.h"

void user_delay_ms(uint32_t period);


void print_sensor_data(struct bme280_data *comp_data);

int8_t set_sensor_data_normal_mode(struct bme280_dev *dev);

int8_t set_sensor_data_forced_mode(struct bme280_dev *dev);



//void bme280_user_init(struct bme280_dev *dev);
void bme280_user_init(void);

void bme280_user_read(void);

void bme280_user_wake(void);
void bme280_user_sleep(void);

#endif /* DRV_BME280_BME280_USER_DEF_H_ */

