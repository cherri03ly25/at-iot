/********************************************************
 *		This file and all its contents are				*
 *		licensed under the MIT v2.0 license.			*
 *		See LICENSE in the root folder for				*
 *		more information.								*														*
 ********************************************************/

/**
 * template.c
 *
 * Created: 2018-07-17
 * Last Modified: 2018-07-31
 * Author(s): Thao Nguyen
 **/

#ifndef DRV_I2C_I2C_H_
#define DRV_I2C_I2C_H_

#include "nrf_drv_twi.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif


/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);
void twi_init (void);
int8_t  user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t  user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);

#endif /* DRV_I2C_I2C_H_ */
