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

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_crypto_init.h"
#include "nrf_crypto_mem.h"
#include "nrf_crypto_aes.h"
#include "nrf_crypto_hmac.h"

#include "encrypt.h"


/* Maximum allowed key = 256 bit */
static uint8_t m_aes_key[32] = {AES_KEY};
static nrf_crypto_key_size_id_t m_aes_key_size = NRF_CRYPTO_KEY_SIZE_256;

static uint8_t m_encrypted_text[AES_DATA_SIZE];
static uint8_t m_decrypted_text[AES_DATA_SIZE];
static uint8_t m_digest[NRF_CRYPTO_HASH_SIZE_SHA256] = {0};

void crypt_init(void){
    ret_code_t err_code;

    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);
}

void crypt_print(uint8_t const * p_text, size_t len){
    NRF_LOG_HEXDUMP_INFO(p_text, len);
    NRF_LOG_FLUSH();
}


uint8_t * crypt_ctr(const uint8_t * p_data,size_t len)
{
    uint8_t     iv[16];
    ret_code_t  err_code;
    size_t      len_in;
    size_t      len_out;

    static nrf_crypto_aes_context_t ctr_encr_128_ctx; // AES CTR encryption context
    static nrf_crypto_aes_context_t ctr_decr_128_ctx; // AES CTR decryption context


    // Init encryption context for 128 bit key
    err_code = nrf_crypto_aes_init(&ctr_encr_128_ctx,
                                  &g_nrf_crypto_aes_ctr_128_info,
                                  NRF_CRYPTO_ENCRYPT);
    AES_ERROR_CHECK(err_code);

    /* Set key for encryption context - only first 128 key bits will be used */
    err_code = nrf_crypto_aes_key_set(&ctr_encr_128_ctx, m_aes_key);
    AES_ERROR_CHECK(err_code);

    memset(iv, 0, sizeof(iv));
    /* Set IV for encryption context */
    err_code = nrf_crypto_aes_iv_set(&ctr_encr_128_ctx, iv);
    AES_ERROR_CHECK(err_code);

    /* Encrypt first 16 bytes */
    err_code = nrf_crypto_aes_update(&ctr_encr_128_ctx,
                                    (uint8_t *)p_data,
                                    AES_DATA_SIZE,           // encrypt first 16 bytes
                                    (uint8_t *)m_encrypted_text);
    AES_ERROR_CHECK(err_code);

    len_out = 0;

    err_code = nrf_crypto_aes_finalize(&ctr_encr_128_ctx,
                                      (uint8_t *)p_data + AES_DATA_SIZE, // skip already encrypted bytes
                                      0,
                                      (uint8_t *)m_encrypted_text + AES_DATA_SIZE,
                                      &len_out);
    AES_ERROR_CHECK(err_code);


    memset(iv, 0, sizeof(iv));

    len_in  = sizeof(m_encrypted_text); // length of encrypted message is an input for decryption
    len_out = sizeof(m_decrypted_text);

    /* Decrypt with integrated function */
    err_code = nrf_crypto_aes_crypt(&ctr_decr_128_ctx,
                                   &g_nrf_crypto_aes_ctr_128_info,
                                   NRF_CRYPTO_DECRYPT,
                                   m_aes_key,
                                   iv,
                                   (uint8_t *)m_encrypted_text,
                                   len_in,
                                   (uint8_t *)m_decrypted_text,
                                   &len_out);
    AES_ERROR_CHECK(err_code);

    if (memcmp(p_data, m_decrypted_text, len_out) == 0)
    {
        //NRF_LOG_INFO("AES CTR executed successfully.");
        return (uint8_t *)m_encrypted_text;
    }
    else
    {
        NRF_LOG_INFO("AES CTR example failed!!!");
        return NULL;
    }

}

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)
uint8_t * crypt_ecb(const uint8_t * p_data,size_t len)
{
    size_t      len_in;
    size_t      len_out;
    ret_code_t  ret_val;

    nrf_crypto_aes_info_t const * p_ecb_info;
    nrf_crypto_aes_context_t      ecb_encr_ctx;
    nrf_crypto_aes_context_t      ecb_decr_ctx;

    memset(m_encrypted_text, 0, sizeof(m_encrypted_text));
    memset(m_decrypted_text, 0, sizeof(m_decrypted_text));

    switch (m_aes_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_ecb_info = &g_nrf_crypto_aes_ecb_128_info;
            //NRF_LOG_INFO("AES key:  128 bit");
            break;


#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_ECB)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_ecb_info = &g_nrf_crypto_aes_ecb_192_info;
            //NRF_LOG_INFO("AES key:  192 bit");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_ecb_info = &g_nrf_crypto_aes_ecb_256_info;
            //NRF_LOG_INFO("AES key:  256 bit");
            break;
#endif

        default:
            NRF_LOG_INFO("AES key: %d bit key size.\nKey size not supported for this backend.",
                            m_aes_key_size);
            return NULL;
    }

    len_in  = len;
    len_out = sizeof(m_decrypted_text);
    /* Encrypt text with integrated function */
    ret_val = nrf_crypto_aes_crypt(&ecb_encr_ctx,
                                   p_ecb_info,
                                   NRF_CRYPTO_ENCRYPT,
                                   m_aes_key,
                                   NULL,
                                   (uint8_t *)p_data,
                                   len_in,
                                   (uint8_t *)m_encrypted_text,
                                   &len_out);

    AES_ERROR_CHECK(ret_val);

    ret_val = nrf_crypto_aes_init(&ecb_decr_ctx,
                                  p_ecb_info,
                                  NRF_CRYPTO_DECRYPT);
    AES_ERROR_CHECK(ret_val);

    /* Set encryption and decryption key */

    ret_val = nrf_crypto_aes_key_set(&ecb_decr_ctx, m_aes_key);
    AES_ERROR_CHECK(ret_val);

    /* Decrypt blocks */
    len_out = sizeof(m_decrypted_text);
    ret_val = nrf_crypto_aes_finalize(&ecb_decr_ctx,
                                      (uint8_t *)m_encrypted_text,
                                      len_in,
                                      (uint8_t *)m_decrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(ret_val);

    if (memcmp(p_data, m_decrypted_text, len_out) == 0)
    {
        //NRF_LOG_INFO("AES ECB executed successfully.");
        return (uint8_t *)m_encrypted_text;
    }
    else
    {
        NRF_LOG_INFO("AES ECB example failed!!!");
        return NULL;
    }
}


uint8_t * crypt_hmac(const uint8_t * p_data,size_t len){
    ret_code_t  err_code;
    static uint8_t m_hmac_key[] = {HMAC_KEY};
    static nrf_crypto_hmac_context_t m_context;
    size_t digest_len = sizeof(m_digest);
    // Initialize frontend (which also initializes backend).
    err_code = nrf_crypto_hmac_init(&m_context,
                                    &g_nrf_crypto_hmac_sha256_info,
                                    m_hmac_key,
                                    sizeof(m_hmac_key));
    APP_ERROR_CHECK(err_code);

    // Push all data in one go (could be done repeatedly)
    err_code = nrf_crypto_hmac_update(&m_context, p_data, len);
    APP_ERROR_CHECK(err_code);

    // Finish calculation
    err_code = nrf_crypto_hmac_finalize(&m_context, m_digest, &digest_len);
    APP_ERROR_CHECK(err_code);
    return m_digest;
}
#endif
