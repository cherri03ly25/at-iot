/********************************************************
 *		This file and all its contents are				*
 *		licensed under the MIT v2.0 license.			*
 *		See LICENSE in the root folder for				*
 *		more information.								*														*
 ********************************************************/

/**
 * template.c
 *
 * Created: 2018-07-21
 * Last Modified: 2018-08-19
 * Author(s): Thao Nguyen
 **/

#ifndef DRV_AES_ENCRYPT_H_
#define DRV_AES_ENCRYPT_H_


#define AES_DATA_SIZE    		16
#define AES_KEY					"I like big ints and I cannot lie"
								//'I',' ','l','i','k','e',' ','b','i','g',' ','i','n','t','s',
								//' ','a','n','d',' ','I',' ','c','a','n','n','o','t',' ','l','i','e'
								//0xc4,0x7b,0x02,0x94,0xdb,0xbb,0xee,0x0f,0xec,0x47,0x57,0xf2,0x2f,0xfe,0xee,0x35,
								//0x87,0xca,0x47,0x30,0xc3,0xd3,0x3b,0x69,0x1d,0xf3,0x8b,0xab,0x07,0x6b,0xc5,0x58

#define TRUNCATED_HMAC_SIZE		11
#define FULL_HMAC_SIZE			32
#define HMAC_KEY				AES_KEY
								//0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b


#define AES_ERROR_CHECK(error)  \
    do {            \
        if (error)  \
        {           \
            NRF_LOG_RAW_INFO("\r\nError = 0x%x\r\n%s\r\n",           \
                             (error),                                \
                             nrf_crypto_error_string_get(error));    \
            return NULL;	\
        }           \
    } while (0);


void crypt_init(void);

void crypt_print(uint8_t const * p_text, size_t hmac_len);

uint8_t * crypt_ctr(const uint8_t * p_data,size_t len);

uint8_t * crypt_ecb(const uint8_t * p_data,size_t len);

uint8_t * crypt_hmac(const uint8_t * p_data,size_t len);


#endif /* DRV_AES_ENCRYPT_H_ */
