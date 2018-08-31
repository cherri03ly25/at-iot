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

#include "boards.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "advertising.h"
#include "sensors.h"
#include "encrypt.h"	// just for testing AES and HMAC, remove if using PACKING
#include "packing.h"

#include "nrf_nvic.h"

APP_TIMER_DEF(m_advdata_update_timer);

static ble_advdata_manuf_data_t manuf_specific_data;
static ble_advdata_t advdata;
static ble_gap_adv_params_t m_adv_params;                                  /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET; /**< Advertising handle used to identify an advertising set. */
static uint8_t              m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];  /**< Buffer for storing an encoded advertising set. */



uint8_t advertised_data[BEACON_DATA_LEN] = {0};                   /**< Information advertised by the Beacon on start up. */


/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = NULL,
        .len    = 0

    }
};



/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)

{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}



/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

}



/**@brief Macro Function for starting advertising.
 */

void advertising_start(void)
{
	ret_code_t err_code;
	err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);	\
	APP_ERROR_CHECK(err_code);
	bsp_board_led_on(BSP_BOARD_LED_0);
}

/**@brief Macro Function for stopping advertising.
 */

void advertising_stop(void)
{
	ret_code_t err_code;
	err_code = sd_ble_gap_adv_stop(m_adv_handle);
	APP_ERROR_CHECK(err_code);
	bsp_board_led_off(BSP_BOARD_LED_0);
}

void advertising_init(void)
{
    uint32_t      err_code;
    //uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    manuf_specific_data.company_identifier = COMPANY_ID;
    manuf_specific_data.data.p_data = (uint8_t *) advertised_data;
    manuf_specific_data.data.size   = BEACON_DATA_LEN;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    //advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr     = NULL;    // Undirected advertisement.
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.duration        = 0;     // Never time out.

    //publish MAC address
    ble_gap_addr_t p_addr;
    p_addr.addr_id_peer = 1;
    p_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
    memcpy(&p_addr.addr, (uint8_t *)0x10001080, 12);
    sd_ble_gap_addr_set(&p_addr);

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling timer timeout for advdata updating event.
 */
void advdata_update_evt_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	ret_code_t err_code;

	bsp_board_led_on(BSP_BOARD_LED_1);

    //read sensors
    sensors_read();
    bsp_board_led_off(BSP_BOARD_LED_0);

    //packing
    pack_beacon_data();

	//empty advdata
    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    //update advertising
    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, NULL);
    APP_ERROR_CHECK(err_code);

    //led indicates when advdata updated
    bsp_board_led_off(BSP_BOARD_LED_1);

    NRF_LOG_INFO("--------------------------------------------");
    NRF_LOG_FLUSH();
}


/**@brief Function for initializing timers. */
void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_advdata_update_timer, APP_TIMER_MODE_REPEATED, advdata_update_evt_handler);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for starting application timer.
*/
void advdata_update_timer_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(m_advdata_update_timer, ADVDATA_UPDATE_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for stopping application timer.
*/
void advdata_update_timer_stop(void)
{
    ret_code_t err_code;
    err_code = app_timer_stop(m_advdata_update_timer);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing Radio Notification Software Interrupts.
 */
void radio_notification_init(void)
{
    ret_code_t err_code;
    uint32_t irq_priority = 2;
    uint8_t notification_type = NRF_RADIO_NOTIFICATION_TYPE_INT_ON_ACTIVE;
	uint8_t notification_distance = NRF_RADIO_NOTIFICATION_DISTANCE_800US;
    err_code = sd_nvic_ClearPendingIRQ(SWI1_IRQn);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_SetPriority(SWI1_IRQn, irq_priority);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_EnableIRQ(SWI1_IRQn);
    APP_ERROR_CHECK(err_code);

    // Configure the event
    err_code = sd_radio_notification_cfg_set(notification_type, notification_distance);
    APP_ERROR_CHECK(err_code);
}

/**@brief Software interrupt 1 IRQ Handler, handles radio notification interrupts.
 */
void SWI1_IRQHandler(bool radio_evt)
{
    if (radio_evt)
    {
    	 bsp_board_led_on(BSP_BOARD_LED_0); //Toggle the status of the LED on each radio notification event
    }
}


