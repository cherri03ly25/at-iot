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
 * Last Modified: 2018-08-19
 * Author(s): Thao Nguyen
 **/

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "advertising.h"
#include "encrypt.h"
#include "sensors.h"
#include "flash_fds.h"

extern sensor_t temp;
extern sensor_t bme280;
extern sensor_t bmi055;
extern sensor_t apds9960;

extern uint8_t advertised_data[BEACON_DATA_LEN];

extern volatile type state;

extern volatile uint32_t ctr;
extern uint8_t ctr_to_flash;

uint32_t htno_32(uint32_t data){
	uint8_t arr[4] = {0};
	uint32_t ret = 0;
	for(int i = 0; i < 4; i++){
		arr[i] = (uint8_t)(data >> 8*i);
		ret |= (uint32_t)arr[i] << 8*(3-i);
	}
	return ret;
}

uint16_t htno_16(uint16_t data){
	uint8_t arr[4] = {0};
	uint16_t ret = 0;
	for(int i = 0; i < 2; i++){
		arr[i] = (uint8_t)(data >> 8*i);
		ret |= (uint32_t)arr[i] << 8*(1-i);
	}
	return ret;
}

void pack_beacon_data(void){
	//static uint32_t ctr = 0;
	uint8_t aes_data[AES_DATA_SIZE] = {0};
	uint8_t * pos = aes_data;
	uint16_t typ = 0;
	memset(pos,0,BEACON_DATA_LEN);

	ctr ++;
	if (ctr%ctr_to_flash==0){
		update_ctr_in_flash(ctr_to_flash);
	}


	uint32_t ret = htno_32(ctr);
	//NRF_LOG_INFO("reversed ctr: 0x%x",htno_32(ctr));

	memcpy(pos, &ret, CTR_LEN);
	pos += CTR_LEN;

	switch(state){
		case BME_THP:
			memcpy(pos, bme280.p_data, bme280.data_len/2);
			pos += bme280.data_len/2;
			typ = BME_TEMP_HUM;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			pos += sizeof(typ);
			memcpy(pos,bme280.p_data + bme280.data_len/2, bme280.data_len/2);
			pos += bme280.data_len/2;
			typ = BME_PRESS;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			break;
		case APDS_COLOR:
			memcpy(pos, apds9960.p_data, 4);
			pos += 4;
			typ = APDS_COLOR1;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			pos += sizeof(typ);
			memcpy(pos,apds9960.p_data + 4, 4);
			pos += 4;
			typ = APDS_COLOR2;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			break;
		case BMI_GYRO_TEMP:
			memcpy(pos, bmi055.p_data, 4);
			pos += 4;
			typ = BMI_GYROXY;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			pos += sizeof(typ);
			memcpy(pos,bmi055.p_data + 4, 2);
			pos += 2;
			memcpy(pos,temp.p_data,temp.data_len);
			pos += temp.data_len;
			typ = BMI_GYROZ_TEMP;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			break;
		case BMI_ACCEL_APDS_PG:
			memcpy(pos, bmi055.p_data+6, 4);
			pos += 4;
			typ = BMI_ACCELXY;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			pos += sizeof(typ);
			memcpy(pos,bmi055.p_data + 10, 2);
			pos += 2;
			memcpy(pos,apds9960.p_data+8,2);
			pos += 2;
			typ = BMI_ACCELZ_ADPS_PG;
			typ = htno_16(typ);
			memcpy(pos, &typ, sizeof(typ));
			break;
		default:
			break;
	}

    state++;
    state = state%NUMBER_OF_STATES;

	//print data before encryption
	crypt_print(aes_data,AES_DATA_SIZE);

	pos = advertised_data;

#define AES		1
#if AES
    //encrypt payload
    uint8_t* p_encrypted = crypt_ecb(aes_data,sizeof(aes_data));
    if(p_encrypted != NULL){
		memcpy(pos, p_encrypted, AES_DATA_SIZE);
		pos += AES_DATA_SIZE;
    }

    NRF_LOG_INFO("%u bytes of encryption:", AES_DATA_SIZE);
    NRF_LOG_FLUSH();
    crypt_print(p_encrypted,AES_DATA_SIZE);
#endif

#define HMAC	1
#if HMAC
    //hash
    uint8_t* p_hmac = crypt_hmac(p_encrypted,AES_DATA_SIZE);
    memcpy(pos, p_hmac, TRUNCATED_HMAC_SIZE);

    NRF_LOG_INFO("%u bytes of hmac:",TRUNCATED_HMAC_SIZE);
    NRF_LOG_FLUSH();
    crypt_print(p_hmac,TRUNCATED_HMAC_SIZE);
#endif

#if !AES && !HMAC
	memcpy(pos, aes_data, AES_DATA_SIZE);
#endif

}

