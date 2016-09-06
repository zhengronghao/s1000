/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sha_dbg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/24/2014 4:31:01 PM
 * Description        : 
 *******************************************************************************/
#include "drv_inc.h"
#include "security_dbg.h"

/*
 * compare_arrays: compares 2 arrays
 * Parameters:
 *  [in] *first: 1st array
 *  [in] *second: 2nd array
 *  [in] length: arrays length
 * Returns:
 *  0 if OK otherwise error
 */
uint8_t compare_arrays(unsigned char *first, unsigned char *second, unsigned length)
{
    unsigned i;  
  
    for(i=0;i<length;i++)
    {
      if(first[i] != second[i])
        return (unsigned char)-1;
    }
    return 0;
}
#ifdef  DBG_SHA1 
/*Note: FIPS 180-2 says SHA-1 and SHA-256 outputs are integers*/
/*
 * . . . the resulting 160-bit message digest of the message, M, is H0 | 
 * H1 | H2 | H3 | H4
 */

/*SHA1 online: http://www.functions-online.com/sha1.html*/

#define SHA1_TEST_CASES      sizeof(sha1_test)/sizeof(sha1_test[0])

typedef struct
{
  unsigned char *input;/*input array*/
  unsigned int output[SHA1_RESULT_LENGTH/4];/*expected result*/
} HashSHA1Test;

/*FSL: testcases*/
const HashSHA1Test sha1_test[] =
{
  {(unsigned char*)"",{0xda39a3ee,0x5e6b4b0d,0x3255bfef,0x95601890,0xafd80709}},
  {(unsigned char*)"a",{0x86f7e437,0xfaa5a7fc,0xe15d1ddc,0xb9eaeaea,0x377667b8}},
  {(unsigned char*)"abc",{0xa9993e36,0x4706816a,0xba3e2571,0x7850c26c,0x9cd0d89d}},
  {(unsigned char*)"message digest",{0xc12252ce,0xda8be899,0x4d5fa029,0x0a47231c,0x1d16aae3}},
  {(unsigned char*)"abcdefghijklmnopqrstuvwxyz",{0x32d10c7b,0x8cf96570,0xca04ce37,0xf2a19d84,0x240d3a89}},
  {(unsigned char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",{0x761c457b,0xf73b14d2,0x7e9e9265,0xc46f4b4d,0xda11f940}},
  {(unsigned char*)"12345678901234567890123456789012345678901234567890123456789012345678901234567890",{0x50abf570,0x6a150990,0xa08b2c5e,0xa40fa0e5,0x85554732}},
  {(unsigned char*)"A00000033301BBE9066D2517511D239C7BFA77884144AE20C7372F515147E8CE6537C54C0A6A4D45F8CA4D290870CDA59F1344EF71D17D3F35D92F3F06778D0D511EC2A7DC4FFEADF4FB1253CE37A7B2B5A3741227BEF72524DA7A2B7B1CB426BEE27BC513B0CB11AB99BC1BC61DF5AC6CC4D831D0848788CD74F6D543AD37C5A2B4C5D5A93B03",{0x49D658C3,0x0D520CD8,0x189FDDF7,0xE7F683E9,0xB2E9BF6A}},
};

/*FSL: new test cases*/
const unsigned int sha1_result1[] = {0xb7a1f498,0xdaf49dce,0x8fb714b4,0xc27fe1a1,0xee33150e};
const unsigned int sha1_result2[] = {0xda39a3ee,0x5e6b4b0d,0x3255bfef,0x95601890,0xafd80709};


#define SHA1_BLOCK_LENGTH                   64

void sha1_self_test(void)
{
    unsigned int sha1_result[SHA1_RESULT_LENGTH/4];
    unsigned int i,length;
    unsigned char *p,*q, pad[SHA1_BLOCK_LENGTH];

    TRACE("\n-|sha1 self test:");
    /*************************************************************************/
    TRACE("\n-|1nd Test: ");
    for(i=0;i<SHA1_TEST_CASES;i++)
    {
        /*get string length*/
        length = strlen((char const *)sha1_test[i].input);
        TRACE("\n\tcase %02d length:%d",i,length);
        sha1(sha1_test[i].input,length,(uint8_t *)sha1_result);
        TRACE("\nsrc:%s",sha1_test[i].input);
        TRACE_BUF("reset",(uint8_t*)sha1_result,sizeof(sha1_result));
        if( compare_arrays((unsigned char *)sha1_result,(unsigned char *)sha1_test[i].output,SHA1_RESULT_LENGTH) ) {
            TRACE("Error during SHA1 testcase %d\n",i);
            return ;
        }
    } 
    /*************************************************************************/
    /*2nd Test*/
    TRACE("\n-|2nd Test: ");
    p = (unsigned char*)"This test tries to use the n-block utility from the hash library";//64 bytes
    q = (unsigned char*)" and as a matter of fact we're trying to get only 128 characters, anyway we need to add 64 more bytes to finally get 192 bytes!!";//128 bytes
    /*calculating length in bits*/
    length = 192<<3;
    /*add padding manually*/
    pad[0] = 0x80;//set bit
    for(i=1;i<56;i++)
        pad[i] = 0;//clear the rest
    /*add length*/
    pad[56] = 0;
    pad[57] = 0;
    pad[58] = 0;
    pad[59] = 0;
    pad[60] = length>>24 & 0xff;
    pad[61] = length>>16 & 0xff;
    pad[62] = length>>8  & 0xff;
    pad[63] = length>>0  & 0xff;
    /*FSL: Initializes the SHA1 state variables*/
    cau_sha1_initialize_output(sha1_result);//init state variables  
    /*FSL: Updates SHA1 state variables for one or more input message blocks*/
    /*note state variables is [IN/OUT] for each nth block, then 512-bit blocks can be non-contiguous*/
    cau_sha1_hash_n(&p[0], 1, sha1_result);//1 block
    cau_sha1_hash_n(&q[0], 2, sha1_result);//2 blocks
    cau_sha1_hash_n(&pad[0], 1, sha1_result);//1 block
    /*FSL: check if output matches with the precalculated result*/
    if( compare_arrays((unsigned char *)sha1_result,(unsigned char *)sha1_result1,SHA1_RESULT_LENGTH) ) {
        TRACE("Error during SHA1 testcase using cau_sha1_hash_n()\n");
        return ;
    }
    /*************************************************************************/
    /*3rd test*/
    TRACE("\n-|3nd Test: ");
    /*using an empty string as test!*/
    /*sha1_result2 is the output*/
    /*calculating length in bits*/
    length = 0<<3;
    /*add padding manually*/
    pad[0] = 0x80;//set bit
    for(i=1;i<56;i++)
        pad[i] = 0;//clear the rest
    /*add length*/    
    pad[56] = 0;
    pad[57] = 0;
    pad[58] = 0;
    pad[59] = 0;
    pad[60] = length>>24 & 0xff;
    pad[61] = length>>16 & 0xff;
    pad[62] = length>>8  & 0xff;
    pad[63] = length>>0  & 0xff; 
    /*FSL: Initializes the SHA1 state variables*/
    cau_sha1_initialize_output(sha1_result);//init state variables
    /* FSL: Performs SHA1 hash algorithm for a single input message block*/
    /* note cau_sha1_initialize_output must be called prior to cau_sha1_hash, 
     * which is different than cau_sha1_update(...)*/
    cau_sha1_hash(&pad[0], sha1_result);
    /*FSL: check if output matches with the precalculated result*/
    if( compare_arrays((unsigned char *)sha1_result,(unsigned char *)sha1_result2,SHA1_RESULT_LENGTH) ) {
        TRACE("Error during SHA1 testcase using cau_sha1_hash()\n");
        return ;
    }
    /*************************************************************************/
    /*FSL: we are done!*/
    TRACE("SHA1 tests completed\n");
}
#endif


#ifdef  DBG_SHA256

#define SHA256_TEST_CASES      sizeof(sha256_test)/sizeof(sha256_test[0])

typedef struct
{
  unsigned char *input;/*input array*/
  unsigned int output[SHA256_RESULT_LENGTH/4];/*expected result*/
} HashSHA256Test;

/*Note: FIPS 180-2 says SHA-1 and SHA-256 outputs are integers*/
/* 
 * ...the resulting 256-bit message digest of the message, M, is H0 | H1 | 
 * H2 | H3 | H4 | H5 | H6 | H7
 */

/*SHA256 online: http://www.xorbin.com/tools/sha256-hash-calculator*/

/*FSL: testcases*/
const HashSHA256Test sha256_test[] =
{
  {(unsigned char*)"",{0xe3b0c442,0x98fc1c14,0x9afbf4c8,0x996fb924,0x27ae41e4,0x649b934c,0xa495991b,0x7852b855}},
  {(unsigned char*)"abc",{0xba7816bf,0x8f01cfea,0x414140de,0x5dae2223,0xb00361a3,0x96177a9c,0xb410ff61,0xf20015ad}},
  {(unsigned char*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",{0x248d6a61,0xd20638b8,0xe5c02693,0x0c3e6039,0xa33ce459,0x64ff2167,0xf6ecedd4,0x19db06c1}},
  {(unsigned char*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",{0xcf5b16a7,0x78af8380,0x036ce59e,0x7b049237,0x0b249b11,0xe8f07a51,0xafac4503,0x7afee9d1}},
  {(unsigned char*)"This is exactly 64 bytes long, not counting the terminating byte",{0xab64eff7,0xe88e2e46,0x165e29f2,0xbce41826,0xbd4c7b35,0x52f6b382,0xa9e7d3af,0x47c245f8}},
  {(unsigned char*)"For this sample, this 63-byte string will be used as input data",{0xf08a78cb,0xbaee082b,0x052ae070,0x8f32fa1e,0x50c5c421,0xaa772ba5,0xdbb406a2,0xea6be342}},
  {(unsigned char*)"And this textual data, astonishing as it may appear, is exactly 128 bytes in length, as are both SHA-384 and SHA-512 block sizes",{0x0ab80334,0x4830f920,0x89494fb6,0x35ad00d7,0x6164ad6e,0x57012b23,0x7722df0d,0x7ad26896}},
  {(unsigned char*)"By hashing data that is one byte less than a multiple of a hash block length (like this 127-byte string), bugs may be revealed.",{0xe4326d04,0x59653d7d,0x3514674d,0x713e74dc,0x3df11ed4,0xd30b4013,0xfd327fdb,0x9e394c26}},
  {(unsigned char*)"The quick brown fox jumps over the lazy dog",{0xd7a8fbb3,0x07d78094,0x69ca9abc,0xb0082e4f,0x8d5651e4,0x6d3cdb76,0x2d02d0bf,0x37c9e592}},
  {(unsigned char*)"This test tries to use the n-block utility from the hash library and as a matter of fact we're trying to get only 128 characters",{0x2ce675bd,0x3b70e104,0xd696d1b2,0x5bf3d42b,0x2b45cd77,0x6d4f590f,0x210f12c4,0x4bf473d5}},
};

/*FSL: new test cases*/
const unsigned int sha256_result1[] = {0xf8c21c3f,0x5f3f5198,0xb22f97d3,0x0e33e341,0x000f6cbc,0x64e79716,0xcaf2c249,0xf77bd8be};
const unsigned int sha256_result2[] = {0xe3b0c442,0x98fc1c14,0x9afbf4c8,0x996fb924,0x27ae41e4,0x649b934c,0xa495991b,0x7852b855};

/*
 * sha256_main: performs SHA256 test using CAU
 */
void sha256_self_test(void)
{
    unsigned int result[SHA256_RESULT_LENGTH/4];
    unsigned int i,length;
    unsigned char *p,*q, pad[SHA256_BLOCK_LENGTH];

    TRACE("\n-|sha256 self test:");
    /*************************************************************************/
    TRACE("\n-|1nd Test: ");
    for(i=0;i<SHA256_TEST_CASES;i++)
    {
        /*get string length*/
        length = strlen((char const *)sha256_test[i].input);
        TRACE("\n\tcase %d length:%03d",i,length);
        sha256(sha256_test[i].input,length,(uint8_t *)result);
        if( compare_arrays((unsigned char *)result,(unsigned char *)sha256_test[i].output,SHA256_RESULT_LENGTH) ) {
            TRACE("Error during SHA256 testcase %d\n",i);
            return ;
        }
    } 
    /*************************************************************************/
    /*2nd Test*/
    TRACE("\n-|2nd Test: ");
    p = (unsigned char*)"This test tries to use the n-block utility from the hash library";//64 bytes
    q = (unsigned char*)" and as a matter of fact we're trying to get only 128 characters, anyway we need to add 64 more bytes to finally get 192 bytes!!";//128 bytes
    /*calculating length in bits*/
    length = 192<<3;
    /*add padding manually*/
    pad[0] = 0x80;//set bit
    for(i=1;i<56;i++)
        pad[i] = 0;//clear the rest
    /*add length*/
    pad[56] = 0;
    pad[57] = 0;
    pad[58] = 0;
    pad[59] = 0;
    pad[60] = length>>24 & 0xff;
    pad[61] = length>>16 & 0xff;
    pad[62] = length>>8  & 0xff;
    pad[63] = length>>0  & 0xff;
    /*FSL: Initializes the SHA256 state variables*/
    cau_sha256_initialize_output(result);//init state variables
    /*FSL: Updates SHA256 state variables for one or more input message blocks*/
    /*note state variables is [IN/OUT] for each nth block, then 512-bit blocks can be non-contiguous*/
    cau_sha256_hash_n(&p[0], 1, result);//1 block
    cau_sha256_hash_n(&q[0], 2, result);//2 blocks
    cau_sha256_hash_n(&pad[0], 1, result);//1 block
    /*FSL: check if output matches with the precalculated result*/
    if( compare_arrays((unsigned char *)result,(unsigned char *)sha256_result1,SHA256_RESULT_LENGTH) ) {
        TRACE("Error during SHA256 testcase using cau_sha256_hash_n()\n");
        return;
    }
    /*************************************************************************/
    /*3rd test*/
    TRACE("\n-|3nd Test: ");
    /*using an empty string as test!*/
    /*calculating length in bits*/
    length = 0<<3;
    /*add padding manually*/
    pad[0] = 0x80;//set bit
    for(i=1;i<56;i++)
        pad[i] = 0;//clear the rest
    /*add length*/    
    pad[56] = 0;
    pad[57] = 0;
    pad[58] = 0;
    pad[59] = 0;
    pad[60] = length>>24 & 0xff;
    pad[61] = length>>16 & 0xff;
    pad[62] = length>>8  & 0xff;
    pad[63] = length>>0  & 0xff; 

    /*FSL: Initializes the SHA256 state variables*/
    cau_sha256_initialize_output(result);//init state variables
    /* FSL: Performs SHA256 hash algorithm for a single input message block*/
    /* note cau_sha256_initialize_output must be called prior to cau_sha256_hash, 
     * which is different than cau_sha256_update(...)*/
    cau_sha256_hash(&pad[0], result);
    /*FSL: check if output matches with the precalculated result*/
    if( compare_arrays((unsigned char *)result,(unsigned char *)sha256_result2,SHA256_RESULT_LENGTH) ) {
        TRACE("Error during SHA256 testcase using cau_sha256_hash()\n"); 
        return;
    }
    /*************************************************************************/

    /*FSL: we are done!*/
    TRACE("SHA256 tests completed\n");
}
#endif


