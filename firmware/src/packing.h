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


#ifndef DRV_PACKING_H_
#define DRV_PACKING_H_


uint32_t htno_32(uint32_t data);

uint16_t htno_16(uint16_t data);

void pack_beacon_data(void);



#endif /* DRV_PACKING_H_ */
