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

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "boards.h"
#include "i2c.h"

#define DEBUG	0
/**
* @brief TWI events handler.
*/
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
   switch (p_event->type)
   {
       case NRF_DRV_TWI_EVT_DONE:
           m_xfer_done = true;
           break;
       default:
           break;
   }
}

/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //MAY WANT TO CHANGE!!!
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}


int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

#if DEBUG
	NRF_LOG_INFO("Reading data from: 0x%x for length of: %d to 0x%x", reg_addr, length, reg_data);
	for(int i = 0; i < length; i++){
		NRF_LOG_INFO("user_i2c_read before read: %x",	reg_data[i]);
	}
#endif

    ret_code_t err_code;
	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&m_twi, dev_id, &reg_addr, 1, true);
    APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	m_xfer_done = false;
	err_code = nrf_drv_twi_rx(&m_twi, dev_id, reg_data, length);
    APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);

#if DEBUG
	for(int i = 0; i < length; i++){
		NRF_LOG_INFO("user_i2c_read after read: %x",	reg_data[i]);
		NRF_LOG_FLUSH();
	}
#endif
	return 0;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

#if DEBUG
	NRF_LOG_INFO("writing data from: 0x%x for length of: %d to 0x%x", reg_addr, length, reg_data)
#endif
	uint8_t data[length + 1];
    data[0] = reg_addr;
	for(uint16_t i = 0; i < length; i++)
		data[i+1] = reg_data[i];

    ret_code_t err_code;
	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&m_twi, dev_id, data ,length + 1, false);
    APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);

	return 0;
}
