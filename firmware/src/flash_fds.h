/************************************************
 *	This file and all its contents are	*
 *	licensed under the MIT license.	*
 *	See LICENSE in the root folder for	*
 *	more information.			*
 ************************************************/

/**
 * flash_fds.h
 *
 * Created: 2018-08-22
 * Last Modified: 2018-08-28
 * Author(s): Teemu Vierros
 **/

#include "fds.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"

#define CTR_FILE     (0xF110)
#define CTR_REC_KEY  (0x7110)

/*Flag to set to delete old record on start up*/
#define DELETE_OLD_RECORD	false

extern volatile uint32_t ctr;
extern uint8_t ctr_to_flash;

void update_ctr_in_flash(int);

void flash_fds_init(void);

