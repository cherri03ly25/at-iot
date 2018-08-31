/********************************************************
 *		This file and all its contents are				*
 *		licensed under the MIT v2.0 license.			*
 *		See LICENSE in the root folder for				*
 *		more information.								*														*
 ********************************************************/

/**
 * template.c
 *
 * Created: 2018-07-05
 * Last Modified: 2018-08_19
 * Author(s): Thao Nguyen
 **/

//#include "custom_board.h"
#include "boards.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "advertising.h"
#include "sensors.h"	//sensors_init
#include "encrypt.h"	//crypt_init

#include "flash_fds.h"

volatile uint32_t ctr = 0;
uint8_t ctr_to_flash = 10;

/**@brief Function for initializing logging. */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing LEDs. */
static void leds_init(void)
{
	bsp_board_init(BSP_INIT_LEDS);
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
        //bsp_board_led_on(BSP_BOARD_LED_0);
    }
}

/**@brief Function for initializing on startup.
 */
void initialize(void)
{
	leds_init();
    log_init();
    timers_init();
    power_management_init();
    ble_stack_init();
    advertising_init();
    radio_notification_init();
    sensors_init();
    crypt_init();
    NRF_LOG_FLUSH();
    flash_fds_init();
}

/***MAIN***/

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
	initialize();
	// Check if counter exist in flash.
	update_ctr_in_flash(ctr_to_flash);
    // Start execution.
    NRF_LOG_INFO("Beacon started!");
    NRF_LOG_FLUSH();
    advertising_start();
    advdata_update_timer_start();


    // Enter main loop.
    while(true)
    {
        idle_state_handle();
    }
}


