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

#ifndef DRV_ADVERTISING_H_
#define DRV_ADVERTISING_H_


#define BEACON_DATA_LEN					27
#define CTR_LEN							4		/**< Length of manufacturer specific data in the advertisement. */
#define TYP_LEN							1

#define COMPANY_ID         		 		0xFFFF                             /**< Manufacturer identifier with value 0xFFFF that is reserved for TESTING ONLY as per www.bluetooth.org. */

#define DEAD_BEEF                       0xDEADBEEF							/**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_BLE_CONN_CFG_TAG            1                                  /**< A tag identifying the SoftDevice BLE configuration. */

#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS)  /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */


#define ADVDATA_UPDATE_INTERVAL APP_TIMER_TICKS(1000)


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

void ble_stack_init(void);

void advertising_start(void);

void advertising_stop(void);

void advertising_init(void);

void timers_init(void);

void advdata_update_timer_start(void);

void advdata_update(void * p_context);

void radio_notification_init(void);

void SWI1_IRQHandler(bool radio_evt);


#endif /* DRV_ADVERTISING_H_ */
