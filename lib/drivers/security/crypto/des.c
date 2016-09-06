/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : des.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/21/2014 11:42:00 AM
 * Description        : 
 *******************************************************************************/
#include "drv_inc.h"
#include "des.h"


static void _cau_des_encrypt(const uint8_t *in, const uint8_t *key,uint8_t *out)
{
    uint8_t input[8];
    uint8_t output[8];
    uint8_t _key[8];

    memcpy(input,in,8);
    memcpy(_key,key,8);
    cau_des_encrypt(input,_key,output);
    memcpy(out,output,8);
}

static void _cau_des_decrypt(const uint8_t *in, const uint8_t *key, uint8_t *out)
{
    uint8_t input[8];
    uint8_t output[8];
    uint8_t _key[8];

    memcpy(input,in,8);
    memcpy(_key,key,8);
    cau_des_decrypt(input,_key,output);
    memcpy(out,output,8);
}

void cau_des3_encrypt(const uint8_t *in, const uint8_t key[24],uint8_t *out)
{
    _cau_des_encrypt(in,key,out);
    _cau_des_decrypt(out,key+8,out);
    _cau_des_encrypt(out,key+16,out);
}

void cau_des3_decrypt(const uint8_t *in, const uint8_t *key,uint8_t *out)
{
    _cau_des_decrypt(in,key+16,out);
    _cau_des_encrypt(out,key+8,out);
    _cau_des_decrypt(out,key,out);
}

void cau_des2_encrypt(const uint8_t *in, const uint8_t key[16],uint8_t *out)
{
    _cau_des_encrypt(in,key,out);
    _cau_des_decrypt(out,key+8,out);
    _cau_des_encrypt(out,key,out);
}

void cau_des2_decrypt(const uint8_t *in, const uint8_t key[16],uint8_t *out)
{
    _cau_des_decrypt(in,key,out);
    _cau_des_encrypt(out,key+8,out);
    _cau_des_decrypt(out,key,out);
}




/*
 * DES-ECB block encryption/decryption
 */
int des1_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[8], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        _cau_des_encrypt(input, key, output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}

int des1_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[8], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        _cau_des_decrypt(input, key, output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}

int des2_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[16], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        cau_des2_encrypt(input,key,output);
//        _cau_des_encrypt(input, key, output);
//        _cau_des_decrypt(output, key+8, output);
//        _cau_des_encrypt(output, key, output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}

int des2_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[16], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        cau_des2_decrypt(input,key,output);
//        _cau_des_decrypt(input,key,output);
//        _cau_des_encrypt(output,key+8,output);
//        _cau_des_decrypt(output,key,output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}

/*
 * 3DES-ECB block encryption/decryption
 */
int des3_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                     const uint8_t key[24], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        cau_des3_encrypt(input, key, output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}

int des3_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                     const uint8_t key[24], uint8_t *output)
{
    if( input_length % DES_BLOCK_LENGTH)
        return (-1);

    while (input_length)
    {
        cau_des3_decrypt(input, key, output);
        input_length -= DES_BLOCK_LENGTH;
        input += DES_BLOCK_LENGTH;
        output += DES_BLOCK_LENGTH;
    }

    return( 0 );
}




typedef void (*CRYPT)(const uint8_t *in, const uint8_t *key,uint8_t *out);

inline int crypt_cbc(const uint8_t *key, uint32_t length,
                     uint8_t iv[8], const uint8_t *input,
                     uint8_t *output, CRYPT cypt)
{
    int i;

    if( length % 8 )
        return( -1);

    while( length > 0 )
    {
        for( i = 0; i < 8; i++ )
            output[i] = (uint8_t)( input[i] ^ iv[i] );

        cypt(output, key, output);
        memcpy( iv, output, 8 );
        input  += 8;
        output += 8;
        length -= 8;
    }
    return( 0 );
}

inline int decrypt_cbc(const uint8_t *key, uint32_t length,
                       uint8_t iv[8],  const uint8_t *input,
                       uint8_t *output, CRYPT cypt)
{
    int i;
    uint8_t temp[8];

    if( length % 8 )
        return( -1);
    while( length > 0 )
    {
        memcpy( temp, input, 8 );
        cypt(output, key, output);

        for( i = 0; i < 8; i++ )
            output[i] = (uint8_t)( output[i] ^ iv[i] );

        memcpy( iv, temp, 8 );

        input  += 8;
        output += 8;
        length -= 8;
    }

    return( 0 );
}

/*
 * DES-CBC buffer encryption/decryption
 */
int des1_encrypt_cbc(const uint8_t key[8], uint8_t iv[8],
                    const uint8_t *input,uint32_t length, 
                    uint8_t *output )
{
    return crypt_cbc(key,length,iv,input,output,_cau_des_encrypt);
}

int des1_decrypt_cbc(const uint8_t key[8], uint8_t iv[8],
                    const uint8_t *input, uint32_t length,
                    uint8_t *output )
{
    return decrypt_cbc(key,length,iv,input,output,_cau_des_decrypt);
}


/*
 * 3DES-CBC buffer encryption/decryption
 */
int des3_encrypt_cbc(const uint8_t key[24], uint8_t iv[8],
                     const uint8_t *input, uint32_t input_length,
                     uint8_t *output )
{
    return crypt_cbc(key,input_length,iv,input,output,cau_des3_encrypt);
}

int des3_decrypt_cbc(const uint8_t key[24], uint8_t iv[8],
                     const uint8_t *input,  uint32_t input_length,
                     uint8_t *output )
{
    return decrypt_cbc(key,input_length,iv,input,output,cau_des3_decrypt);
}

int des2_encrypt_cbc(const uint8_t key[16], uint8_t iv[8],
                     const uint8_t *input, uint32_t input_length,
                     uint8_t *output )
{
    return crypt_cbc(key,input_length,iv,input,output,cau_des2_encrypt);
}

int des2_decrypt_cbc(const uint8_t key[16], uint8_t iv[8],
                     const uint8_t *input,  uint32_t input_length,
                     uint8_t *output )
{
    return decrypt_cbc(key,input_length,iv,input,output,cau_des2_decrypt);
}



//-----------------------------------------------
/** Encrypt a block of data (CBC)
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       array of two DES keys
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 * @param  iv        IV (0 if NULL)
 *
 */
void des_encrypt_cbc(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys, uint8_t * iv)
{
    if (numKeys == 2)
    {
        des2_encrypt_cbc(key,iv,inData,length,outData);
    } else if (numKeys == 3)
    {
        des3_encrypt_cbc(key,iv,inData,length,outData);
    } else 
    {
        des1_encrypt_cbc(key,iv,inData,length,outData);
    }
}

/** Decrypt a block of data (CBC) Triple DES.
 *  Uses Triple DES with two keys 
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       array of two DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 * @param  iv        IV (0 if NULL)
 * Note:   For CBC Decrypt, inData and outData must not be the same array
 */
void des_decrypt_cbc(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys, uint8_t * iv)
{
    if (numKeys == 2)
    {
        des2_decrypt_cbc(key,iv,inData,length,outData);
    } else if (numKeys == 3)
    {
        des3_decrypt_cbc(key,iv,inData,length,outData);
    } else 
    {
        des1_decrypt_cbc(key,iv,inData,length,outData);
    }
}

/** Single-DES encrypt blocks of data (ECB)..
 * Uses one key
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 */
void des_encrypt_ecb(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys)
{
    if (numKeys == 2)
    {
        des2_encrypt_ecb(inData,length,key,outData);
    } else if (numKeys == 3)
    {
        des3_encrypt_ecb(inData,length,key,outData);
    } else 
    {
        des1_encrypt_ecb(inData,length,key,outData);
    }
}


/** DES decrypt blocks of data (ECB).
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 */
void des_decrypt_ecb(const uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys)
{
    if (numKeys == 2)
    {
        des2_decrypt_ecb(inData,length,key,outData);
    } else if (numKeys == 3)
    {
        des3_decrypt_ecb(inData,length,key,outData);
    } else 
    {
        des1_decrypt_ecb(inData,length,key,outData);
    }
}




#ifdef  DBG_DES
/*
 * DES and 3DES test vectors:
 *
 */
static const uint8_t des3_test_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

static const uint8_t des3_test_iv[8] =
{
    0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
};

static const uint8_t des3_test_buf[8] =
{
    0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
};

static const uint8_t des3_test_ecb_dec[3][8] =
{
    { 0xCD, 0xD6, 0x4F, 0x2F, 0x94, 0x27, 0xC1, 0x5D },
    { 0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB },
    { 0x83, 0x25, 0x39, 0x76, 0x44, 0x09, 0x1A, 0x0A }
};

static const uint8_t des3_test_ecb_enc[3][8] =
{
    { 0x6A, 0x2A, 0x19, 0xF4, 0x1E, 0xCA, 0x85, 0x4B },
    { 0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42 },
    { 0xDD, 0x17, 0xE8, 0xB8, 0xB4, 0x37, 0xD2, 0x32 }
};

static const uint8_t des3_test_cbc_dec[3][8] =
{
    { 0x12, 0x9F, 0x40, 0xB9, 0xD2, 0x00, 0x56, 0xB3 },
    { 0x47, 0x0E, 0xFC, 0x9A, 0x6B, 0x8E, 0xE3, 0x93 },
    { 0xC5, 0xCE, 0xCF, 0x63, 0xEC, 0xEC, 0x51, 0x4C }
};

static const uint8_t des3_test_cbc_enc[3][8] =
{
    { 0x54, 0xF1, 0x5A, 0xF6, 0xEB, 0xE3, 0xA4, 0xB4 },
    { 0x35, 0x76, 0x11, 0x56, 0x5F, 0xA1, 0x8E, 0x4D },
    { 0xCB, 0x19, 0x1F, 0x85, 0xD1, 0xED, 0x84, 0x39 }
};

/*
 * Checkup routine
 */
int des_self_test( int verbose )
{
    int i, j, u, v;
    unsigned char key[24];
    unsigned char buf[8];
    unsigned char prv[8];
    unsigned char iv[8];

    memset( key, 0, 24 );

    TRACE("\n");
    /*
     * ECB mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            TRACE( "  DES%c-ECB-%3d (%s): ",
                    ( u == 0 ) ? ' ' : '3', 56 + u * 56,
                    ( v == DES_DECRYPT ) ? "dec" : "enc" );
        memcpy( buf, des3_test_buf, 8 );
        switch( i )
        {
        case 0:
        case 1:
            memcpy(key,des3_test_keys,8);
            break;
        case 2:
        case 3:
            memcpy(key,des3_test_keys,16);
            memcpy(key+16,des3_test_keys,8);
            break;
        case 4:
        case 5:
            memcpy(key,des3_test_keys,24);
            break;
        default:
            return( 1 );
        }

        if(v == DES_ENCRYPT ) {
            for( j = 0; j < 10000; j++ ) {
                if( u == 0 )
                    des1_encrypt_ecb(buf,8,key,buf );
                else
                    cau_des3_encrypt(buf,key,buf);
            }
        } else {
            for( j = 0; j < 10000; j++ ) {
                if( u == 0 )
                    des1_decrypt_ecb(buf,8,key,buf);
                else
                    cau_des3_decrypt(buf,key,buf );
            }
        }

        if( ( v == DES_DECRYPT &&
              memcmp( buf, des3_test_ecb_dec[u], 8 ) != 0 ) ||
            ( v != DES_DECRYPT &&
              memcmp( buf, des3_test_ecb_enc[u], 8 ) != 0 ) )
        {
            if( verbose != 0 )
                TRACE( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            TRACE( "passed\n" );
    }

    if( verbose != 0 )
        TRACE( "\n" );

    /*
     * CBC mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            TRACE( "  DES%c-CBC-%3d (%s): ",
                    ( u == 0 ) ? ' ' : '3', 56 + u * 56,
                    ( v == DES_DECRYPT ) ? "dec" : "enc" );
        memcpy( iv,  des3_test_iv,  8 );
        memcpy( prv, des3_test_iv,  8 );
        memcpy( buf, des3_test_buf, 8 );

        switch( i )
        {
        case 0:
        case 1:
            memcpy(key,des3_test_keys,8);
            break;
        case 2:
        case 3:
            memcpy(key,des3_test_keys,16);
            memcpy(key+16,des3_test_keys,8);
            break;
        case 4:
        case 5:
            memcpy(key,des3_test_keys,24);
            break;
        default:
            return( 1 );
        }

        if( v == DES_DECRYPT )
        {
            for( j = 0; j < 10000; j++ ) {
                if( u == 0 )
                    des1_decrypt_cbc(key,iv, buf,8, buf );
                else
                    des3_decrypt_cbc(key,iv, buf,8, buf );
            }
        } else
        {
            for( j = 0; j < 10000; j++ ) {
                unsigned char tmp[8];
                if( u == 0 )
                    des1_encrypt_cbc(key,iv, buf,8,buf );
                else
                    des3_encrypt_cbc(key,iv, buf,8,buf );
                memcpy( tmp, prv, 8 );
                memcpy( prv, buf, 8 );
                memcpy( buf, tmp, 8 );
            }
            memcpy( buf, prv, 8 );
        }
        if( ( v == DES_DECRYPT &&
              memcmp( buf, des3_test_cbc_dec[u], 8 ) != 0 ) ||
            ( v != DES_DECRYPT &&
              memcmp( buf, des3_test_cbc_enc[u], 8 ) != 0 ) )
        {
            if( verbose != 0 )
                TRACE( "failed\n" );
            return( 1 );
        }
        if( verbose != 0 )
            TRACE( "passed\n" );
    }

    if( verbose != 0 )
        TRACE( "\n" );

    return( 0 );
}
#endif



