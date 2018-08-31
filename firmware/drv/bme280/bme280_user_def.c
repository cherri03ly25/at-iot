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


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

#include "bme280.h"
#include "sensors.h"	// sensor_t
#include "packing.h"	// to use htno functions

#define DEBUG	0

struct bme280_dev dev;
struct bme280_data comp_data;

extern sensor_t bme280;

void user_delay_ms(uint32_t period)
{
	nrf_delay_ms(period);
}


void print_sensor_data(struct bme280_data *comp_data)
{
	NRF_LOG_INFO("bme280 temp %ld (0x%x)", comp_data->temperature, comp_data->temperature);
	NRF_LOG_INFO("bme280 hum %ld (0x%x)",comp_data->humidity/10, comp_data->humidity/10);
	NRF_LOG_INFO("bme280 press %ld (0x%x)",comp_data->pressure, comp_data->pressure);
}

int8_t set_sensor_data_normal_mode(struct bme280_dev *dev)
{
	int8_t rslt;
	uint8_t settings_sel;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);
	//dev->delay_ms(40);
	return rslt;
}

int8_t set_sensor_data_forced_mode(struct bme280_dev *dev)
{
  int8_t rslt;
  uint8_t settings_sel;

  /* Recommended mode of operation: Indoor navigation */
  dev->settings.osr_h = BME280_OVERSAMPLING_1X;
  dev->settings.osr_p = BME280_OVERSAMPLING_16X;
  dev->settings.osr_t = BME280_OVERSAMPLING_2X;
  dev->settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  rslt = bme280_set_sensor_settings(settings_sel, dev);
  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
  //dev->delay_ms(40);
  return rslt;
}


void bme280_user_init(void)
{
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_ms = user_delay_ms;

	bme280_init(&dev);
	//set sensor normal mode
	set_sensor_data_forced_mode(&dev);
	NRF_LOG_INFO("BME280 initialized!");
    NRF_LOG_FLUSH();
}


void bme280_user_read(void)
{
	bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	dev.delay_ms(40);
	bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
	print_sensor_data(&comp_data);
	uint8_t * pos = (uint8_t *) bme280.p_data;
	uint16_t temperature = htno_16((uint16_t)comp_data.temperature);
	memcpy(pos,&temperature,2);
	pos +=2;

#if DEBUG
	NRF_LOG_INFO("bme280 temperature %d 0x%x", htno_16(temperature), htno_16(temperature));
#endif

	uint16_t hum = htno_16((uint16_t)comp_data.humidity/10);
	memcpy(pos, &hum, 2);
	pos += 2;

#if DEBUG
	NRF_LOG_INFO("bme280 humidity %d 0x%x", htno_16(hum), htno_16(hum));
#endif

	uint32_t press = htno_32(comp_data.pressure);
	memcpy(pos, &press, 4);

#if DEBUG
	NRF_LOG_INFO("bme280 pressure %d 0x%x", htno_32(press), htno_32(press));
#endif

}
#endif /* DRV_BME280_BME280_USER_DEF_H_ */

