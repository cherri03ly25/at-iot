/************************************************
 *	This file and all its contents are	*
 *	licensed under the MIT license.	*
 *	See LICENSE in the root folder for	*
 *	more information.			*
 ************************************************/

/**
 * flash_fds.c
 *
 * Created: 2018-08-22
 * Last Modified: 2018-08-28
 * Author(s): Teemu Vierros
 **/

#include "flash_fds.h"

uint32_t ctr_count = 0;

/* A record containing counter data. */
static fds_record_t const m_ctr_record =
{
    .file_id           = CTR_FILE,
    .key               = CTR_REC_KEY,
    .data.p_data       = &ctr_count,
    /* The length of a record is always 4-byte units (aka words). */
    .data.length_words = (sizeof(ctr_count) + 3) / sizeof(uint32_t),
};

/* Flag to check if flash data storage is initialized. */
static bool volatile fds_initialized;

/* Flag to check if deleting record is done. */
static bool volatile fds_deleted;

/* Flag to check if garbage collecting is done. */
static bool volatile fds_garbace_collected;

/* Flash data storage event handler. */
static void fds_evt_handler(fds_evt_t const * p_evt)
{
    NRF_LOG_INFO("fds event received by the handler id:%x",
                  p_evt->id);

    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
        {
        	NRF_LOG_INFO("Flash data storage initialized");
        	fds_initialized = true;
        } break;

        case FDS_EVT_WRITE:
        {
			NRF_LOG_INFO("Record writen to file: 0x%04x",    p_evt->write.file_id);
        } break;

        case FDS_EVT_DEL_RECORD:
        {
			NRF_LOG_INFO("Record deleted from file: 0x%04x",    p_evt->del.file_id);
			fds_deleted = true;
        } break;
        case FDS_EVT_GC:
        	fds_garbace_collected = true;
            break;
        default:
            break;
    }
    NRF_LOG_FLUSH();
}

/* Brief function for deleting records*/
bool delete_record(int file,int key)
{
	ret_code_t ret_code;
    fds_find_token_t  tok   = {0};
    fds_record_desc_t desc  = {0};

    ret_code = fds_record_find(file, key, &desc, &tok);

    if (ret_code == 0)
    {
        ret_code_t ret_code = fds_record_delete(&desc);
        if (ret_code == 0)
        {
            return true;
        }
    }
    return false;

}
/* Brief function for waiting for flash to initialize.*/
static void wait_for_fds_ready(void)
{
    while (!fds_initialized)
    {
    	(void) sd_app_evt_wait();
    }
}
/*Brien function to log flash statics*/
void flash_statistics(void)
{
	ret_code_t ret_code;
    NRF_LOG_INFO("Reading flash usage statistics...");
    NRF_LOG_FLUSH();

    fds_stat_t stat = {0};

    ret_code = fds_stat(&stat);
    APP_ERROR_CHECK(ret_code);

    NRF_LOG_INFO("Found %d open, %d valid, %d dirty records(ready to be garbage collected)",\
    		stat.open_records, stat.valid_records, stat.dirty_records);

    NRF_LOG_FLUSH();
}

/* Brief function for to checking and cleaning dirty records*/
void check_garbage_statistics(void)
{
    fds_stat_t stat = {0};
    ret_code_t ret_code = fds_stat(&stat);
    APP_ERROR_CHECK(ret_code);
    if(stat.dirty_records>=450){
    	fds_garbace_collected = false;
    	NRF_LOG_INFO("Cleaning up %d dirty records in flash", stat.dirty_records);
        NRF_LOG_FLUSH();
    	fds_gc();
    	while (!fds_garbace_collected)
    	{
    		(void) sd_app_evt_wait();
    	}
    }
}

/*Brief function to check if a record exist if it dose update it else create it. */
void update_ctr_in_flash(int update_amount){
	check_garbage_statistics();

	fds_record_desc_t desc = {0};
	fds_find_token_t  tok  = {0};
	ret_code_t ret_code;
	ret_code = fds_record_find(CTR_FILE, CTR_REC_KEY, &desc, &tok);

	if (ret_code == 0)
	{
		/* Updating counter file that is already in flash.*/
		fds_flash_record_t flash_ctr = {0};

		/* Open the records. */
		ret_code = fds_record_open(&desc, &flash_ctr);
		APP_ERROR_CHECK(ret_code);

		/* Copy the counter from flash. */
		memcpy(&ctr_count, flash_ctr.p_data, sizeof(uint32_t));

		NRF_LOG_INFO("Counter file found, updating counter from %d to %d.", ctr_count,(ctr_count+update_amount));
		NRF_LOG_FLUSH();

		/* Update counter count. */
		ctr_count+=update_amount;
		/* Update non flash counter */
		ctr = (uint32_t)ctr_count;

		/* Close the record. */
		ret_code = fds_record_close(&desc);
		APP_ERROR_CHECK(ret_code);

		/* Write the updated record to flash. */
		ret_code = fds_record_update(&desc, &m_ctr_record);
		APP_ERROR_CHECK(ret_code);
	}
	else
	{
		/* Counter not found. Writing a new counter. */
		NRF_LOG_INFO("Writing Counter file...");
		NRF_LOG_FLUSH();

		ret_code = fds_record_write(&desc, &m_ctr_record);
		APP_ERROR_CHECK(ret_code);
	}
}

/* Brief function for initializing the flash data storage*/
void flash_fds_init(void)
{
    ret_code_t ret_code;

    /* Registering the event handler to get events. */
    (void) fds_register(fds_evt_handler);

    NRF_LOG_INFO("Initializing fds...");
    ret_code = fds_init();
    APP_ERROR_CHECK(ret_code);

    /* Wait for flash data storage to initialize. */
    wait_for_fds_ready();
    flash_statistics();

#if DELETE_OLD_RECORD
	NRF_LOG_INFO("Deleting old counter file and garbage collecting.");
	delete_record(CTR_FILE, CTR_REC_KEY);
	while (!fds_deleted)
	{
		(void) sd_app_evt_wait();
	}
	fds_gc();
	NRF_LOG_INFO("Old Counter file deleted and garbage collected.");
	NRF_LOG_FLUSH();
#endif

}


