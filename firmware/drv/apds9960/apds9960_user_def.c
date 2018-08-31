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

#if !SparkFun_APDS9960
#include "Adafruit_APDS9960.h"
#else
#include "SparkFun_APDS9960.h"
#define APDS9960_INTERRUPT 	1
#include "nrf_drv_gpiote.h"
#endif


extern sensor_t apds9960;

extern volatile type state;


void apds9960_user_init(void)
{
	if(!Adafruit_APDS9960_begin(10, APDS9960_AGAIN_4X, APDS9960_ADDRESS)){
	  NRF_LOG_INFO("Failed to initialize APDS9960!");
	  NRF_LOG_FLUSH();
	}
	else{
		NRF_LOG_INFO("APDS9960 initialized!");
		NRF_LOG_FLUSH();
	}
	Adafruit_APDS9960_enableProximity(true);
	//Adafruit_APDS9960_enableGesture(true); //TODO: check
	Adafruit_APDS9960_enableColor(true);	//TODO: check
}

void apds9960_user_wake(void)
{
	  Adafruit_APDS9960_enable(true);
	  nrf_delay_ms(20);
}
void apds9960_user_sleep(void)
{
	  Adafruit_APDS9960_enable(false);
	  //nrf_delay_ms(10);
}
void apds9960_user_read(void)
{
	uint8_t * pos = (uint8_t *) apds9960.p_data;
#if !STOP
	uint16_t r, g, b, c;
	//wait for color data to be ready
	while(Adafruit_APDS9960_colorDataReady()){ //TODO: check
	  nrf_delay_ms(5);
	}
	//get the data and print the different channels
	Adafruit_APDS9960_getColorData(&r, &g, &b, &c);
	uint16_t color[4];
	color[0] = htno_16(r);
	color[1] = htno_16(g);
	color[2] = htno_16(b);
	color[3] = htno_16(c);
	memcpy(pos, color, sizeof(color));
	pos += sizeof(color);
	if(state == APDS_COLOR){
		//float cct=Adafruit_APDS9960_calculateColorTemperature(r,g,b);
		NRF_LOG_INFO("apds9960 red %d (0x%x)",r,r);
		NRF_LOG_INFO("apds9960 green %d (0x%x)",g,g);
		NRF_LOG_INFO("apds9960 blue %d (0x%x)",b,b);
		NRF_LOG_INFO("apds9960 clear %d (0x%x)",c,c);
		//NRF_LOG_INFO("cct: %f",cct);
	}
#endif
#if !STOP
	*pos = Adafruit_APDS9960_readProximity();
	if(state == BMI_ACCEL_APDS_PG){
		NRF_LOG_INFO("apds9960 proximity %d (0x%x)",*pos);
	}
	pos++;
#endif
#if STOP
	*pos = Adafruit_APDS9960_readGesture();
	if(state == BMI_ACCEL_APDS_PG){
		switch(*pos){
			case APDS9960_DOWN:
				NRF_LOG_INFO("DOWN");
				break;
			case APDS9960_UP:
				NRF_LOG_INFO("UP");
				break;
			case APDS9960_LEFT:
				NRF_LOG_INFO("LEFT");
				break;
			case APDS9960_RIGHT:
				NRF_LOG_INFO("RIGHT");
				break;
			default:
				NRF_LOG_INFO("gesture = 0x%x",*pos);
				break;
		}
	}
#endif

}


