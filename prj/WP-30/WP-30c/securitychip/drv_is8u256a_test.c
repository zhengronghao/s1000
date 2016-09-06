#include "wp30_ctrl.h"
//#include "drv_is8u256a.h"

#if defined(DEBUG_Dx)
#define DS_DEBUG_MAIN   0
#define DS_SC           (DBG_INFO /* | DBG_LABEL_FUNC | DBG_LABEL_LINE */)    //!< ���Կ���
#define DS_SC_DATA      (DBG_INFO /* | DBG_LABEL_FUNC | DBG_LABEL_LINE */)    //!< ���ݵ��Կ���
#define DS_SC_ERR       (DBG_INFO /* | DBG_LABEL_FUNC | DBG_LABEL_LINE */)    //!< �����������
#else
#define DS_DEBUG_MAIN   0
#endif /* defined(DEBUG_Dx) */
#include "libdebug.h"

#if defined(CFG_SECURITY_CHIP)
#if defined(DEBUG_Dx)

#if 0
extern int sc_is8u_clean_cache(void);

int test_gpio_ctrl(void * p)
{
    int pin = (int)p;
    int c = 0;

//    dbg_out_I(DS_SC, "Select pin: %#x", pin);
//    set_pio_perigh(pin, 0, 0);
//    dbg_out_I(DS_SC, "Set level(0/1):");
//    dbg_in_N(&c);
//    dbg_out_I(DS_SC, "Level: %d", c);
//    set_pio_output(pin, c % 2);

    return 0;
}
int test_gpio(void * p)
{
//    dbg_test_setlist(
//        { "SPI0 MOSI",  (void *)(PIN_SPI0_MOSI),    test_gpio_ctrl, },
//        { "SPI0 MISO",  (void *)(PIN_SPI0_MISO),    test_gpio_ctrl, },
//        { "SPI0 CLK",   (void *)(PIN_SPI0_CLK),     test_gpio_ctrl, },
//        { "NCS2(PD21)", (void *)(PIOD_PIN_(21)),    test_gpio_ctrl, },
//    );
    return 0;
}
int test_spi_send(void * p)
{
    int c = 0;
    int t = 0;
    int i = 0;

    dbg_out_I(DS_SC, "Set data(hex, oct or int):");
    dbg_in_N((char *)&c);
    dbg_out_I(DS_SC, "Get character: %#x", c);
    dbg_out_I(DS_SC, "Set send time.");
    dbg_in_N(&t);
    dbg_out_I(DS_SC, "Send time: %d", t);
    for(i = 0; i < t; i++) {
        dbg_out_I(DS_SC, "%d.Send: %#x, ret: %#x", i, c,
                sc_spi_rw_byte(c));
        sys_delay_ms(2000);
    }

    return 0;
}
typedef struct {
    char * d;
    int l;
}test_data_segment_t;
int test_send_data_segment(void * p)
{
    test_data_segment_t * s = (test_data_segment_t *)p;
    int i = 0;
    char * d = s->d;
    for(i = 0; i < s->l; i++, d++) {
        dbg_out_I(DS_SC, "%d. Write: %#x; Read: %#x",
                i, *d, sc_spi_rw_byte(*d));
    }

    return 0;
}
int test_send_custom_byte(void * p)
{
    int n = 0;
    dbg_out_I(DS_SC, "Input a number:");
    dbg_in_N(&n);
    dbg_out_I(DS_SC, "Write: %#x, Read: %#x", n, sc_spi_rw_byte(n));
    return 0;
}
int test_data_segment(void * p)
{
    test_data_segment_t dummy = { "\xFF\xFF\xFF\xFF", 4, };
    test_data_segment_t chk_st = { "\xE5\xFF", 2, };
    test_data_segment_t send_pre = { "\xC1", 1, };
    test_data_segment_t recv_pre = { "\xA1", 1, };
    test_data_segment_t fac_code = { "\xBF\x18\x00\x00\x08", 5, };
    test_data_segment_t init_chkst = { "\xFB\x33\x44\x55\x66", 5, };
    dbg_test_setlist(
        { "0xFF(read)(4)",          &dummy,     test_send_data_segment,     },
        { "Check status(2)",        &chk_st,    test_send_data_segment,     },
        { "Send prepare(1)",        &send_pre,  test_send_data_segment,     },
        { "Send custom byte(1)",    NULL,       test_send_custom_byte,      },
        { "Receive prepare(1)",     &recv_pre,  test_send_data_segment,     },
        { "----",                   NULL,       NULL,                       },
        { "Get fac code(5)",        &fac_code,  test_send_data_segment,     },
        { "Init(check status)(5)",  &init_chkst,test_send_data_segment,     },
        );
    return 0;
}
int test_init(void * p)
{
    sc_is8u256a_init();
    return 0;
}
int test_getstatus(void * p)
{
    unsigned int ret = sc_is8u_spi_status(0);
    dbg_out_I(DS_SC, "Get status: %s(%#x)", (ret == 0) ? "Idle" :
            (ret == -SC_IS8U_RET_BUSY) ? "Busy" : "Unknown", ret);
    return 0;
}
int test_get_fac_code(void * p)
{
    char buf[256] = { 0 };
    int len = sc_is8u_get_fac_code((unsigned char *)buf);
    if(len > 0 && len < sizeof(buf)) {
        dbg_dmp_HCL(DS_SC, buf, len, "Get factory code");
    }
    else {
        dbg_out_E(DS_SC_ERR, "Read failed! (%#x)", -len);
    }
    
    return 0;
}
int test_get_random_data(void * p)
{
    int i = 0;
    int ret = 0;
    char buf[256] = { 0 };
    for(i = 0; i < 20; i++) {
        memset(buf, 0x00, sizeof(buf));
        ret = sc_is8u_get_random_data(i, (unsigned char *)buf);
        if(ret > 0) {
            dbg_dmp_HCL(DS_SC, buf, ret, "Get random data");
        }
    }
    return 0;
}

typedef struct {
    char * desc;
    int size;
    char * ek;
    char * ak;
    char * iv;
    char * pt;
    char * ct;
}SM1_TEST_DATA_t;
int test_sm1_process(void * p)
{
    int i = 0;
    int ret = 0;
    SM1_TEST_DATA_t * dat = (SM1_TEST_DATA_t *)p;
    char plaintext[256] = { 0 };
    char ciphertext[256] = { 0 };
    dbg_out_H(DS_SC, "#### DESC: %s", dat->desc);
    dbg_dmp_HCL(DS_SC, dat->ek, dat->size, "EK");
    dbg_dmp_HCL(DS_SC, dat->ak, dat->size, "AK");
    dbg_dmp_HCL(DS_SC, dat->iv, dat->size, "IV");
    dbg_dmp_HCL(DS_SC, dat->pt, dat->size, "Plain text");
    for(i = SM_ECB_MODE; i <= SM_CBC_MODE; i++) {
        switch(i) {
            case SM_ECB_MODE: {
                dbg_out_H(DS_SC, "#### mode: ECB");
                break;
            }
            case SM_CBC_MODE: {
                dbg_out_H(DS_SC, "#### mode: CBC");
                break;
            }
        }
        if((ret = sc_is8u_sm1_enc(i, (unsigned char *)dat->pt, dat->size,
                        (unsigned char *)dat->ek, (unsigned char *)dat->iv,
                        (unsigned char *)ciphertext))) {
            dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
            return -1;
        }
        dbg_dmp_HCL(DS_SC, ciphertext, dat->size, "Encrypted text");
        if(memcmp(dat->ct, ciphertext, dat->size)) {
            dbg_out_W(DS_SC_ERR, "Cipher text and expect is different!");
            dbg_dmp_HCL(DS_SC, dat->ct, dat->size, "Expect cipher text");
        }
        if((ret = sc_is8u_sm1_dec(i, (unsigned char *)ciphertext, dat->size,
                        (unsigned char *)dat->ek, (unsigned char *)dat->iv,
                        (unsigned char *)plaintext))) {
            dbg_out_E(DS_SC_ERR, "Decrypt failed!(%d)", ret);
            return -1;
        }
        dbg_dmp_HCL(DS_SC, plaintext, dat->size, "Decrypted text");
        if(memcmp(dat->pt, plaintext, dat->size)) {
            dbg_out_W(DS_SC_ERR, "Plain text and decrypt text is different!");
            dbg_dmp_HCL(DS_SC, dat->pt, dat->size, "Plain text");
        }
    }
    return 0;
}
int test_sm1(void * p)
{
    SM1_TEST_DATA_t test1 = {
        "SM1 test1", 16,
        "\x17\xcf\x3d\xb8\x8b\xc3\xdf\x1c\xad\xa1\x31\xe3\x2d\xeb\xc1\xa1",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x37\x7d\x48\xc0\x25\xbe\x1e\xda\xa5\x3f\xfc\x86\xae\x14\x80\xfd",
        "\xfb\x1d\xa8\x18\x05\x8e\x8b\x1b\x20\x05\xa0\x1c\xb7\x32\x40\x6e",
    };
    SM1_TEST_DATA_t test2 = {
        "SM1 test2", 16,
        "\x7c\xa7\x43\x05\x18\xcb\x91\x9d\xba\x0c\x73\x3d\xe8\xc5\xb1\x43",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x37\x7d\x48\xc0\x25\xbe\x1e\xda\xa5\x3f\xfc\x86\xae\x14\x80\xfd",
        "\xbf\x8b\xca\x6e\x96\x70\x46\x35\xbb\xce\x6b\x0e\x04\x2a\xc9\x4d",
    };
    SM1_TEST_DATA_t test3 = {
        "SM1 test3", 16,
        "\x37\x7d\x48\xc0\x25\xbe\x1e\xda\xa5\x3f\xfc\x86\xae\x14\x80\xfd",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x7c\xa7\x43\x05\x18\xcb\x91\x9d\xba\x0c\x73\x3d\xe8\xc5\xb1\x43",
        "\xe5\x7c\x33\x57\xd6\x4b\x0a\x0c\x32\x68\x68\xe3\x90\xaa\x2c\xb2",
    };
    SM1_TEST_DATA_t test4 = {
        "SM1 test4", 16,
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    };
    dbg_test_setlist(
        { "test1",                  &test1,     test_sm1_process,           },
        { "test2",                  &test2,     test_sm1_process,           },
        { "test3",                  &test3,     test_sm1_process,           },
        { "test4",                  &test4,     test_sm1_process,           },
        );

    return 0;
}

typedef struct {
    int len;
    int id_len;
    unsigned char * pubk;
    unsigned char * prik;
    unsigned char * pt;
    unsigned char * id;
}SM2_TEST_DATA_t;
int test_sm2_process(void * p)
{
    int ret = 0;
    SM2_TEST_DATA_t * dat = (SM2_TEST_DATA_t *)p;
    unsigned char plaintext[256] = { 0 };
    unsigned char ciphertext[256] = { 0 };
    unsigned char sign[256] = { 0 };
    dbg_dmp_HCL(DS_SC, (char *)dat->pubk, 64, "Public key");
    dbg_dmp_HCL(DS_SC, (char *)dat->prik, 32, "Private key");
    dbg_dmp_HCL(DS_SC, (char *)dat->pt, dat->len, "Plain text");

    if((ret = sc_is8u_sm2_import_keypair(dat->prik, dat->pubk)) < 0) {
        dbg_out_E(DS_SC_ERR, "Import key pair failed!(%d)", ret);
        return -1;
    }

    dbg_out_I(DS_SC, "________ SM2 Encrypt ________");
    if((ret = sc_is8u_sm2_enc(dat->len, (unsigned char *)dat->pt,
                    (unsigned char *)ciphertext)) < 0) {
        dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)ciphertext, ret, "Encrypted text");
    dbg_out_I(DS_SC, "________ SM2 Decrypt ________");
    if((ret = sc_is8u_sm2_dec(ret, (unsigned char *)ciphertext,
                    (unsigned char *)plaintext)) < 0) {
        dbg_out_E(DS_SC_ERR, "Decrypt failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)plaintext, dat->len, "Decrypted text");
    if(memcmp(dat->pt, plaintext, dat->len)) {
        dbg_out_W(DS_SC_ERR, "Plain text and decrypt text is different!");
        dbg_dmp_HCL(DS_SC, (char *)dat->pt, dat->len, "Plain text");
    }

    dbg_out_I(DS_SC, "________ SM2 Sign ________");
    if((ret = sc_is8u_sm2_sign(dat->id_len, (unsigned char *)dat->id, dat->len,
                    (unsigned char *)dat->pt, (unsigned char *)sign)) < 0) {
        dbg_out_E(DS_SC_ERR, "Sign failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)sign, ret, "Sign");
    dbg_out_I(DS_SC, "________ SM2 Verify ________");
    if((ret = sc_is8u_sm2_verify(dat->id_len, (unsigned char *)dat->id, dat->len,
                    (unsigned char *)dat->pt, (unsigned char *)sign)) < 0) {
        dbg_out_E(DS_SC_ERR, "Verify plain text and sign failed!(%d)", ret);
        return -1;
    }

    return 0;
}
int test_sm2(void * p)
{
    SM2_TEST_DATA_t test1 = {
        28, 32,
        (unsigned char *)
        "\x25\xBA\x1E\x01\x4C\x13\xCA\x86\x16\xCB\xD4\x73\x92\xEE\xFA\xA6"
        "\xFE\x6C\x1B\x0F\xA8\x17\x16\xD4\xA0\x93\x1D\xC9\x2A\xC0\x51\xC4"
        "\x4D\x45\x09\x47\x55\xED\x45\x9B\x78\x74\xD4\xA5\x5C\xD0\x95\x50"
        "\x4D\xFB\x7B\x51\xD0\x45\x00\xD4\xDC\x43\x58\x99\x13\x21\xEE\xEA",
        (unsigned char *)
        "\xFD\x71\x83\xFA\x4A\xD6\x39\xCB\x48\x4F\x62\xBF\x43\x8C\xD9\xAC"
        "\x8B\x04\x5B\xC5\x38\x05\x78\xE8\xCA\xB1\x5A\xCE\x8E\xEE\x8C\x07",
        (unsigned char *)
        "\x68\x65\x6C\x6C\x6F\x2C\x48\x55\x41\x20\x44\x41\x20\x58\x49\x4E"
        "\x20\x41\x4E\x2C\x53\x4D\x32\x20\x54\x45\x53\x54",
        (unsigned char *)
        "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
        "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",
    };
    SM2_TEST_DATA_t test2 = {
        140, 32,
        (unsigned char *)
        "\x25\xBA\x1E\x01\x4C\x13\xCA\x86\x16\xCB\xD4\x73\x92\xEE\xFA\xA6"
        "\xFE\x6C\x1B\x0F\xA8\x17\x16\xD4\xA0\x93\x1D\xC9\x2A\xC0\x51\xC4"
        "\x4D\x45\x09\x47\x55\xED\x45\x9B\x78\x74\xD4\xA5\x5C\xD0\x95\x50"
        "\x4D\xFB\x7B\x51\xD0\x45\x00\xD4\xDC\x43\x58\x99\x13\x21\xEE\xEA",
        (unsigned char *)
        "\xFD\x71\x83\xFA\x4A\xD6\x39\xCB\x48\x4F\x62\xBF\x43\x8C\xD9\xAC"
        "\x8B\x04\x5B\xC5\x38\x05\x78\xE8\xCA\xB1\x5A\xCE\x8E\xEE\x8C\x07",
        (unsigned char *)
        "\x68\x65\x6C\x6C\x6F\x2C\x48\x55\x41\x20\x44\x41\x20\x58\x49\x4E"
        "\x20\x41\x4E\x2C\x53\x4D\x32\x20\x54\x45\x53\x54\x68\x65\x6C\x6C"
        "\x6F\x2C\x48\x55\x41\x20\x44\x41\x20\x58\x49\x4E\x20\x41\x4E\x2C"
        "\x53\x4D\x32\x20\x54\x45\x53\x54\x68\x65\x6C\x6C\x6F\x2C\x48\x55"
        "\x41\x20\x44\x41\x20\x58\x49\x4E\x20\x41\x4E\x2C\x53\x4D\x32\x20"
        "\x54\x45\x53\x54\x68\x65\x6C\x6C\x6F\x2C\x48\x55\x41\x20\x44\x41"
        "\x20\x58\x49\x4E\x20\x41\x4E\x2C\x53\x4D\x32\x20\x54\x45\x53\x54"
        "\x68\x65\x6C\x6C\x6F\x2C\x48\x55\x41\x20\x44\x41\x20\x58\x49\x4E"
        "\x20\x41\x4E\x2C\x53\x4D\x32\x20\x54\x45\x53\x54",
        (unsigned char *)
        "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
        "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",
    };
    dbg_test_setlist(
        { "test1",          &test1, test_sm2_process,   },
        { "test2",          &test2, test_sm2_process,   },
        );
    return 0;
}

typedef struct {
    int len;
    char * data;
    char * digest_sha1;
    char * digest_sha256;
    char * digest_sm3_160;
    char * digest_sm3_192;
    char * digest_sm3_256;
}DIGEST_TEST_DATA_t;
int test_digest_process(void * p)
{
    int ret = 0;
    DIGEST_TEST_DATA_t * d = (DIGEST_TEST_DATA_t *)p;
    unsigned char digest[256] = { 0 };

    dbg_dmp_HCL(DS_SC, (char *)d->data, d->len, "Encrypted text");
    if((ret = sc_is8u_sha1(d->len, (unsigned char *)d->data, digest)) < 0) {
        dbg_out_E(DS_SC_ERR, "Cipher failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)digest, 20, "SHA1");
    if(memcmp(d->digest_sha1, digest, 20)) {
        dbg_out_W(DS_SC_ERR, "SHA1 different!");
        dbg_dmp_HCL(DS_SC, (char *)d->digest_sha1, 20, "Expect val");
    }
    if((ret = sc_is8u_sha256(d->len, (unsigned char *)d->data, digest)) < 0) {
        dbg_out_E(DS_SC_ERR, "Cipher failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)digest, 32, "SHA256");
    if(memcmp(d->digest_sha256, digest, 32)) {
        dbg_out_W(DS_SC_ERR, "SHA256 different!");
        dbg_dmp_HCL(DS_SC, (char *)d->digest_sha256, 32, "Expect val");
    }
    if((ret = sc_is8u_sm3(d->len, (unsigned char *)d->data, digest)) < 0) {
        dbg_out_E(DS_SC_ERR, "Cipher failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)digest, 32, "SM3 256bit");
    if(memcmp(d->digest_sm3_256, digest, 32)) {
        dbg_out_W(DS_SC_ERR, "SM3 256bit different!");
        dbg_dmp_HCL(DS_SC, (char *)d->digest_sm3_256, 32, "Expect val");
    }

    return 0;
}
int test_digest(void *p)
{
    DIGEST_TEST_DATA_t test1 = {
        128,
        "\x76\x3A\xFC\x53\x7F\x78\x76\xC2\xD0\xB5\x9F\xDB\x68\xD7\x62\xE9"
        "\x0C\xEF\xD2\x22\xBB\x35\x8D\x0D\x69\x31\x86\x7C\xE2\x65\x38\x64"
        "\x9B\xE3\x57\x9A\x40\x04\x48\x3E\xA5\xD8\x4D\x00\x50\x63\xF7\x6F"
        "\xB1\xCE\x7E\x5F\x2F\x93\x3B\x5E\xD7\x57\xA7\x18\x18\x2F\x38\x3C"
        "\x4D\x58\x29\x1A\x6A\x5D\x8D\x07\xC0\x81\xF6\x68\x06\x03\x15\x39"
        "\x09\x33\x62\xD8\x54\x88\x3A\x88\x74\xF7\xB9\x19\x92\x5D\xAB\xC7"
        "\x4C\x17\x3E\x21\x62\xF0\x7E\x67\x80\xE3\x11\xFF\x0A\xEF\x05\x9A"
        "\xE6\x20\x30\x3D\xEC\xB6\x28\x9E\x97\xF7\x2C\x01\x87\x23\xC4\x71",
        "\xC1\x4F\x3D\x0A\xA6\xFF\x75\xFA\xB9\x1D\xC5\xFB\x60\xF1\xB7\x0F"
        "\x7F\x78\xEB\x35",
        "\x52\x81\x92\x26\x91\xED\xC7\xE1\xBC\x31\x97\x8E\x6E\xA3\x8D\x08"
        "\x48\x07\x20\xBE\x3D\xD3\xBB\x4E\xB6\x8F\x6C\xA4\x94\x69\x08\x8A",
        "\x86\x92\x37\x05\x9F\xA7\xEB\x3F\x19\xAA\xA3\x17\x05\xC6\xF3\x37"
        "\x73\xCD\xCC\xF0",
        "\x86\x92\x37\x05\x13\x92\xF3\xAD\x19\xAA\xA3\x17\x05\xC6\xF3\x37"
        "\xA4\xEE\x8D\xDE\x73\xCD\xCC\xF0",
        "\xd3\x2b\x9d\x79\x3b\x49\x66\xe1\x8c\x35\x18\x92\xe6\x52\x77\x75"
        "\x6e\xf0\xcc\x9d\x28\xdb\x95\x4c\x95\x9f\xbb\x85\xe3\x94\x84\x42",
    };
    DIGEST_TEST_DATA_t test2 = {
        3,
        "\x61\x62\x63",
        "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2\x6C"
        "\x9C\xD0\xD8\x9D",
        "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22\x23"
        "\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00\x15\xAD",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x66\xC7\xF0\xF4\x62\xEE\xED\xD9\xD1\xF2\xD4\x6B\xDC\x10\xE4\xE2"
        "\x41\x67\xC4\x87\x5C\xF2\xF7\xA2\x29\x7D\xA0\x2B\x8F\x4B\xA8\xE0",
    };
    DIGEST_TEST_DATA_t test3 = {
        55,
        "\x2F\x91\x74\x20\xE7\x02\xDB\xA9\x70\xC0\x71\xAE\x49\x71\xAD\x08"
        "\xDE\x3D\x7D\x0D\x90\xDC\x1E\x33\x4E\xD2\x04\x44\xE5\x4F\x10\x9B"
        "\xA8\x0D\xD2\x2F\x25\xC2\x4F\xAA\x83\xD5\xAD\x58\x68\x7F\x1A\xA6"
        "\x8F\x1B\x74\x9D\x0A\xD9\x99",
        "\x31\x09\xFB\x04\xFD\x1A\xF6\x71\x27\x15\x31\x56\xA2\x6C\xEB\xB4"
        "\x2F\x4C\x3C\x0B",
        "\x6F\xD8\x94\xD6\x4B\xD3\xAA\x07\xB0\x15\x0C\xBC\x8D\xD9\x22\xE8"
        "\xF3\x41\xAD\x2D\x37\x39\xC2\xCB\xEA\xB7\x77\x02\x7B\x27\x6F\x26",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\xd1\xe5\x44\x80\x1f\x8b\x29\xac\xfb\xc8\xcf\x69\x9f\x0d\xa3\x7b"
        "\xf3\x18\x74\xeb\x66\xa0\xeb\x50\xae\xee\x4a\x4d\x31\x58\x7c\x2f",
    };
    dbg_test_setlist(
        { "test1",                  &test1,     test_digest_process,        },
        { "test2",                  &test2,     test_digest_process,        },
        { "test3",                  &test3,     test_digest_process,        },
        );
    return 0;
}

typedef struct {
    int len;
    unsigned char * key;
    unsigned char * iv;
    unsigned char * pt;
    unsigned char * ct;
}SM4_TEST_DATA_t;
int test_sm4_process(void * p)
{
    int ret = 0;
    SM4_TEST_DATA_t * dat = (SM4_TEST_DATA_t *)p;
    unsigned char plaintext[4096] = { 0 };
    unsigned char ciphertext[4096] = { 0 };
    dbg_dmp_HCL(DS_SC, (char *)dat->key, 16, "KEY");
    dbg_dmp_HCL(DS_SC, (char *)dat->iv, 16, "IV");
    dbg_dmp_HCL(DS_SC, (char *)dat->pt, dat->len, "Plain text");
    if((ret = sc_is8u_sm4_enc(SM_ECB_MODE, (unsigned char *)dat->pt, dat->len,
                    (unsigned char *)dat->key, (unsigned char *)dat->iv,
                    (unsigned char *)ciphertext))) {
        dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)ciphertext, dat->len, "Encrypted text");
    if(memcmp(dat->ct, ciphertext, dat->len)) {
        dbg_out_W(DS_SC_ERR, "Cipher text and expect is different!");
        dbg_dmp_HCL(DS_SC, (char *)dat->ct, dat->len, "Expect cipher text");
    }
    if((ret = sc_is8u_sm4_dec(SM_ECB_MODE, (unsigned char *)ciphertext,
                    dat->len, (unsigned char *)dat->key,
                    (unsigned char *)dat->iv, (unsigned char *)plaintext))) {
        dbg_out_E(DS_SC_ERR, "Decrypt failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)plaintext, dat->len, "Decrypted text");
    if(memcmp(dat->pt, plaintext, dat->len)) {
        dbg_out_W(DS_SC_ERR, "Plain text and decrypt text is different!");
        dbg_dmp_HCL(DS_SC, (char *)dat->pt, dat->len, "Plain text");
    }
    return 0;
}
int test_sm4(void * p)
{
    SM4_TEST_DATA_t test1 = {
        80,
        (unsigned char *)
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10",
        (unsigned char *)
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        (unsigned char *)
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10"
        "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11"
        "\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22"
        "\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33"
        "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12",
        (unsigned char *)
        "\x68\x1E\xDF\x34\xD2\x06\x96\x5E\x86\xB3\xE9\x4F\x53\x6E\x42\x46"
        "\x6B\x36\x33\xA5\xED\x04\xF5\xAB\xD5\x19\x78\x70\xB5\x50\x66\x42"
        "\x7C\x3B\xC4\xEB\x1C\x18\xE7\xF1\xD8\x79\xEA\x60\x2E\xEC\x46\xCF"
        "\x99\xCF\xE9\xD2\xB4\xF8\x8C\xB9\xD5\x20\x49\xF7\x58\xE2\x30\xD9"
        "\x70\x62\x8D\x12\x92\x3E\x19\x2B\xDF\x4A\xBE\xAD\x23\x03\x14\x6D",
    };
    dbg_test_setlist(
        { "test1",                  &test1,     test_sm4_process,           },
        );

    return 0;
}

int test_gen_keypair(void *p)
{
    int i = 0;
    int ret;
    unsigned char pubk[64];
    unsigned char prik[32];
    char disp[4] = { 0 };
    dbg_out_I(DS_SC, " >> Display details? (y/n)");
    dbg_in_S(disp, 1);
    if(disp[0] == 'Y' || disp[1] == 'y') {
        dbg_out_I(DS_SC, " << Display.");
        disp[0] = 1;
    }
    else {
        dbg_out_I(DS_SC, " << No details.");
        disp[0] = 0;
    }
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    for(; i < 256; i++) {
        ret = sc_is8u_sm2_gen_keypair();
        if(ret) {
            dbg_out_E(DS_SC_ERR, "Generate key pair failed!");
            break;
        }
        ret = sc_is8u_sm2_export_keypair(prik, pubk);
        if(ret) {
            dbg_out_E(DS_SC_ERR, "Export key pair failed!");
            break;
        }
        if(disp[0]) {
            dbg_out_I(DS_SC, "#### %d", i);
            dbg_dmp_HCL(DS_SC, (char *)prik, 32, "Private key");
            dbg_dmp_HCL(DS_SC, (char *)pubk, 64, "Public key");
        }
        else {
            dbg_out(DS_SC, "\r%d", i);
        }
    }
    time = sys_get_counter() - time;
    float speed = (float)256 / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:256, speed:%f Pair/S", time, speed);
    return 0;
}
int test_speed_sm2_enc(void *p)
{
    int ret = 0;
    /// ��������
    int len = 0;
    int testtime = 0;
    dbg_out_I(DS_SC, " >> Input plain text length:(0, 1024]");
    dbg_in_N(&len);
    if(len <= 0) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", len);
        return -1;
    }
    dbg_out_I(DS_SC, " << Get length: %d", len);
    dbg_out_I(DS_SC, " >> Input test time:");
    dbg_in_N(&testtime);
    dbg_out_I(DS_SC, " << Get test time: %d", testtime);
    /// ��Կ����
    unsigned char pubk[64] = { 0 };
    unsigned char prik[32] = { 0 };
    dbg_out_I(DS_SC, "Generate key pair...");
    ret = sc_is8u_sm2_gen_keypair();
    if(ret) {
        dbg_out_E(DS_SC_ERR, "Generate key pair failed!");
        return -1;
    }
    ret = sc_is8u_sm2_export_keypair(prik, pubk);
    if(ret) {
        dbg_out_E(DS_SC_ERR, "Export key pair failed!");
        return -1;
    }
    /// ��������
    int i;
    int j;
    unsigned char plaintext[1024] = { 0 };
    unsigned char ciphertext[4096] = { 0 };
    for(i = 0; i < 1024; i++) {
        for(j = 0; j < 256; j++, i++) {
            plaintext[i] = j;
        }
    }
    dbg_dmp_HCL(DS_SC, (char *)pubk, 64, "Public key");
    dbg_dmp_HCL(DS_SC, (char *)prik, 32, "Private key");
    dbg_dmp_HCL(DS_SC, (char *)plaintext, len, "Plain text");
    /// �ٶȲ���
    unsigned int totalbyte = 0;
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    for(i = 0; i < testtime; i++) {
        if((ret = sc_is8u_sm2_enc(len, plaintext, ciphertext)) < 0) {
            dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
            return -1;
        }
        totalbyte += len;
        dbg_out(DS_SC, "\r%d", i);
    }
    time = sys_get_counter() - time;
    /// ���Խ�����
    dbg_out_H(DS_SC, "Cipher text, length: %d", ret);
    dbg_dmp_HC(DS_SC, (char *)ciphertext, ret);
    float speed = (float)totalbyte / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:%d Byte, speed:%f Byte/S",
            time, totalbyte, speed);
    return 0;
}
int test_speed_sm2_dec(void *p)
{
    int ret = 0;
    /// ��������
    int len = 0;
    int testtime = 0;
    dbg_out_I(DS_SC, " >> Input plain text length:(0, 1024]");
    dbg_in_N(&len);
    if(len <= 0) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", len);
        return -1;
    }
    dbg_out_I(DS_SC, " << Get length: %d", len);
    dbg_out_I(DS_SC, " >> Input test time:");
    dbg_in_N(&testtime);
    dbg_out_I(DS_SC, " << Get test time: %d", testtime);
    /// ��Կ����
    unsigned char pubk[64] = { 0 };
    unsigned char prik[32] = { 0 };
    dbg_out_I(DS_SC, "Generate key pair...");
    ret = sc_is8u_sm2_gen_keypair();
    if(ret) {
        dbg_out_E(DS_SC_ERR, "Generate key pair failed!");
        return -1;
    }
    ret = sc_is8u_sm2_export_keypair(prik, pubk);
    if(ret) {
        dbg_out_E(DS_SC_ERR, "Export key pair failed!");
        return -1;
    }
    /// ��������
    int i;
    int j;
    unsigned char plaintext[1024] = { 0 };
    unsigned char ciphertext[4096] = { 0 };
    for(i = 0; i < 1024; i++) {
        for(j = 0; j < 256; j++, i++) {
            plaintext[i] = j;
        }
    }
    dbg_dmp_HCL(DS_SC, (char *)pubk, 64, "Public key");
    dbg_dmp_HCL(DS_SC, (char *)prik, 32, "Private key");
    dbg_dmp_HCL(DS_SC, (char *)plaintext, len, "Plain text");
    /// ��������
    if((ret = sc_is8u_sm2_enc(len, plaintext, ciphertext)) < 0) {
        dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
        return -1;
    }
    len = ret;
    memset(plaintext, 0x00, sizeof(plaintext));
    dbg_dmp_HCL(DS_SC, (char *)ciphertext, len, "Cipher text");
    /// �ٶȲ���
    unsigned int totalbyte = 0;
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    for(i = 0; i < testtime; i++) {
        if((ret = sc_is8u_sm2_dec(len, ciphertext, plaintext)) < 0) {
            dbg_out_E(DS_SC_ERR, "Decrypt failed!(%d)", ret);
            return -1;
        }
        totalbyte += len;
        dbg_out(DS_SC, "\r%d", i);
    }
    time = sys_get_counter() - time;
    /// ���Խ�����
    dbg_out_H(DS_SC, "Plain text, length: %d", ret);
    dbg_dmp_HC(DS_SC, (char *)plaintext, ret);
    float speed = (float)totalbyte / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:%d Byte, speed:%f Byte/S",
            time, totalbyte, speed);
    return 0;
}
int test_speed_digest(void *p)
{
    struct sc_digest_t {
        int (* init)(void);
        int (* update)(int, unsigned char *);
        int (* finish)(unsigned char *);
    };
    static struct sc_digest_t s_digest_methods[] = {
        { sc_is8u_sha1_init, sc_is8u_sha1_update, sc_is8u_sha1_finish, },
        { sc_is8u_sha256_init, sc_is8u_sha256_update, sc_is8u_sha256_finish, },
        { sc_is8u_sm3_init, sc_is8u_sm3_update, sc_is8u_sm3_finish, },
    };
    unsigned char str[1024] = { 0 };
    /// ��������
    int type;
    int len;
    int testtime;
    dbg_out_I(DS_SC, " >> Input digest type:(0:SHA1, 1:SHA256, 2:SM3)");
    dbg_in_N(&type);
    if(type < 0 || type > 2) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", type);
        return -1;
    }
    dbg_out_I(DS_SC, " << Get type: %d", type);
    dbg_out_I(DS_SC, " >> Input plain text length:(0, 192]");
    dbg_in_N(&len);
    if(len <= 0 || len > 192) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", len);
        return -1;
    }
    dbg_out_I(DS_SC, " << Get length: %d", len);
    dbg_out_I(DS_SC, " >> Input test time:");
    dbg_in_N(&testtime);
    dbg_out_I(DS_SC, " << Get test time: %d", testtime);
    /// ��������
    int i;
    int j;
    for(i = 0; i < 1024; i++) {
        for(j = 0; j < 256; j++, i++) {
            str[i] = j;
        }
    }
    dbg_dmp_HCL(DS_SC, (char *)str, len, "Plain text");
    /// �ٶȲ���
    unsigned int totalbyte = 0;
    unsigned char digest[256] = { 0 };
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    s_digest_methods[type].init();
    for(i = 0; i < testtime; i++) {
        s_digest_methods[type].update(len, str);
        totalbyte += len;
        dbg_out(DS_SC, "\r%d", i);
    }
    int ret = s_digest_methods[type].finish(digest);
    time = sys_get_counter() - time;
    /// ������
    dbg_dmp_HCL(DS_SC, (char *)digest, ret, "Digest");
    float speed = (float)totalbyte / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:%d Byte, speed:%f Byte/S",
            time, totalbyte, speed);

    return 0;
}
int test_speed_sm4_enc(void *p)
{
    int ret = 0;
    /// ��������
    int len = 0;
    dbg_out_I(DS_SC, " >> Input plain text length:(0, 4096]");
    dbg_in_N(&len);
    if(len <= 0 || len > 4096) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", len);
        return -1;
    }
    len = (len >> 4) << 4;
    dbg_out_I(DS_SC, " << Get length: %d", len);
    /// ��������
    int i;
    int j;
    unsigned char key[32] = {
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10",
    };
    unsigned char iv[32] = {
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    };
    unsigned char plaintext[4096 + 4] = { 0 };
    unsigned char ciphertext[4096 + 4] = { 0 };
    for(i = 0; i < 4096; i++) {
        for(j = 0; j < 256; j++, i++) {
            plaintext[i] = j;
        }
        i--;
    }
    dbg_dmp_HCL(DS_SC, (char *)plaintext, len, "Plain text");
    /// �ٶȲ���
    unsigned int totalbyte = 0;
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    if((ret = sc_is8u_sm4_enc(SM_ECB_MODE, plaintext,
                    len, key, iv, ciphertext))) {
        dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
        return -1;
    }
    totalbyte += len;
    time = sys_get_counter() - time;
    /// ���Խ�����
    dbg_dmp_HCL(DS_SC, (char *)ciphertext, len, "Cipher text");
    float speed = (float)totalbyte / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:%d Byte, speed:%f Byte/S",
            time, totalbyte, speed);
    return 0;
}
int test_speed_sm4_dec(void *p)
{
    int ret = 0;
    /// ��������
    int len = 0;
    dbg_out_I(DS_SC, " >> Input plain text length:(0, 4096]");
    dbg_in_N(&len);
    if(len <= 0 || len > 4096) {
        dbg_out_W(DS_SC_ERR, " << Bad input: %d", len);
        return -1;
    }
    len = (len >> 4) << 4;
    dbg_out_I(DS_SC, " << Get length: %d", len);
    /// ��������
    int i;
    int j;
    unsigned char key[32] = {
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10",
    };
    unsigned char iv[32] = {
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    };
    unsigned char plaintext[4096 + 4] = { 0 };
    unsigned char ciphertext[4096 + 4] = { 0 };
    for(i = 0; i < 4096; i++) {
        for(j = 0; j < 256; j++, i++) {
            plaintext[i] = j;
        }
        i--;
    }
    if((ret = sc_is8u_sm4_enc(SM_ECB_MODE, plaintext,
                    len, key, iv, ciphertext))) {
        dbg_out_E(DS_SC_ERR, "Encrypt failed!(%d)", ret);
        return -1;
    }
    dbg_dmp_HCL(DS_SC, (char *)plaintext, len, "Plain text");
    dbg_dmp_HCL(DS_SC, (char *)ciphertext, len, "Cipher text");
    memset(plaintext, 0x00, len);
    /// �ٶȲ���
    unsigned int totalbyte = 0;
    dbg_out_I(DS_SC, "Start...\n");
    unsigned int time = sys_get_counter();
    if((ret = sc_is8u_sm4_dec(SM_ECB_MODE, ciphertext,
                    len, key, iv, plaintext))) {
        dbg_out_E(DS_SC_ERR, "Cipher failed!(%d)", ret);
        return -1;
    }
    totalbyte += len;
    time = sys_get_counter() - time;
    /// ���Խ�����
    dbg_dmp_HCL(DS_SC, (char *)plaintext, len, "Plain text");
    float speed = (float)totalbyte / ((float)time / (float)1000);
    dbg_out_I(DS_SC, "Time:%d ms, total:%d Byte, speed:%f Byte/S",
            time, totalbyte, speed);
    return 0;
}

int sc_is8u_test(void)
{
    sc_is8u256a_init();
    dbg_test_setlist(
        { "==== Chip test",         NULL,   NULL,                   },
        { "IS8U init",              NULL,   test_init,              },
        { "Get chip status",        NULL,   test_getstatus,         },
        { "Get fac code",           NULL,   test_get_fac_code,      },
        { "Get random data",        NULL,   test_get_random_data,   },
        { "==== Algorithm test",    NULL,   NULL,                   },
        { "SM1 test",               NULL,   test_sm1,               },
        { "SM2 test",               NULL,   test_sm2,               },
        { "SHA1 SHA256 SM3 test",   NULL,   test_digest,            },
        { "SM4 test",               NULL,   test_sm4,               },
        { "==== Hard test",         NULL,   NULL,                   },
        { "GPIO test",              NULL,   test_gpio,              },
        { "SPI test",               NULL,   test_spi_send,          },
        { "Data segment test",      NULL,   test_data_segment,      },
        { "==== Performance test",  NULL,   NULL,                   },
        { "Gen key",                NULL,   test_gen_keypair,       },
        { "SM2 enc",                NULL,   test_speed_sm2_enc,     },
        { "SM2 dec",                NULL,   test_speed_sm2_dec,     },
        { "Digest",                 NULL,   test_speed_digest,      },
        { "SM4 enc",                NULL,   test_speed_sm4_enc,     },
        { "SM4 dec",                NULL,   test_speed_sm4_dec,     },
        { "==== Other test",        NULL,   NULL,                   },
        );
    return 0;
}

#endif
/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : 
 * bfief              : 
 * Author             : 
 * Email              : �����Բ���
 * Version            : 
 * Date               : 
 * Description        : 
 *******************************************************************************/
enum {
    SMST_RET_OK = 0,
    SMST_RET_SM2_KEYGEN_ERR,
    SMST_RET_SM2_ENC_ERR,
    SMST_RET_SM2_DEC_ERR,
    SMST_RET_SM2_SIGN_ERR,
    SMST_RET_SM2_VERIFY_ERR,

    SMST_RET_SM3_ERR,

    SMST_RET_SM4_ENC_ERR,
    SMST_RET_SM4_DEC_ERR,
    SMST_RET_SM4_PINBLOCK_ERR,

    SMST_RET_RAND_ERR,
};

static unsigned char s_sm2_prik[] = {       //!< 自检SM2私钥
    0xb6, 0x02, 0x0b, 0xd0, 0x40, 0xbe, 0xd5, 0xe3,
    0x7b, 0x68, 0x69, 0x38, 0xaf, 0xf5, 0xad, 0xdc,
    0x42, 0x9a, 0x84, 0xc8, 0x9d, 0x1e, 0x0a, 0x26,
    0xdc, 0x80, 0x84, 0x16, 0xa3, 0x19, 0x88, 0x12,
};
static unsigned char s_sm2_pubk[] = {       //!< 自检SM2公钥
    0xe4, 0xad, 0x07, 0xb4, 0x64, 0xb0, 0xd6, 0x78,
    0xfd, 0x18, 0xed, 0x48, 0x3c, 0x83, 0x05, 0x9b,
    0xa1, 0x72, 0x06, 0xaf, 0xf7, 0xb3, 0x29, 0xff,
    0xf3, 0xfb, 0x09, 0x6b, 0x71, 0xab, 0x6e, 0xee,
    0xa4, 0x4b, 0xd5, 0x5c, 0x31, 0x81, 0x7a, 0x3d,
    0x30, 0x5c, 0x6f, 0x8f, 0x87, 0x10, 0x22, 0xeb,
    0xe6, 0xb6, 0x79, 0xe5, 0xe0, 0x9a, 0xf9, 0x1b,
    0x70, 0x56, 0x47, 0x77, 0x93, 0x9d, 0x16, 0xdc,
};
static unsigned char s_sm2_pt[] = {         //!< 自检SM2明文
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",
};
static unsigned char s_sm2_ct[] = {         //!< 自检SM2密文
    0xed, 0xf4, 0x0a, 0x8f, 0x24, 0x98, 0x5b, 0xb7,
    0x3e, 0x10, 0xfc, 0xda, 0x47, 0xca, 0x92, 0xa6,
    0x31, 0x0a, 0x02, 0x6f, 0xdc, 0xf4, 0x12, 0x65,
    0x71, 0x08, 0xb5, 0xb2, 0x39, 0xb7, 0x52, 0x9f,
    0x2f, 0x8e, 0xd7, 0xf0, 0x87, 0x30, 0x68, 0x8a,
    0x50, 0xd2, 0x9b, 0x37, 0xb2, 0xe3, 0xb2, 0x84,
    0x58, 0xe5, 0x37, 0x74, 0x57, 0x50, 0xda, 0x80,
    0x56, 0x3c, 0xed, 0xe9, 0x98, 0x20, 0x33, 0xfd,
    0x1c, 0x49, 0xef, 0xf1, 0x68, 0x2e, 0xea, 0x41,
    0x0a, 0xd9, 0xda, 0x49, 0x0c, 0x26, 0x79, 0x04,
    0xc8, 0x80, 0xe0, 0x0a, 0xc6, 0x43, 0xce, 0xaa,
    0x3a, 0xac, 0x1b, 0xa3, 0xbd, 0x5e, 0x29, 0x27,
    0x37, 0xd5, 0x84, 0xab, 0x94, 0x30, 0x8e, 0x86,
    0xcf, 0xb4, 0x1e, 0x83, 0x37, 0x02, 0x31, 0x27,
    0x83, 0x4f, 0x86, 0x51, 0x3b, 0xe7, 0xf9, 0x05,
    0x10, 0x20, 0x32, 0x1b, 0x18, 0x5f, 0xc3, 0x8f,
    0x2f, 0xf2, 0xe8, 0x86,
};
static unsigned char s_sm2_id[] = { //!< 自检签名ID
    "11111111111111111111111111111111",
};
static unsigned char s_sm2_sign[] = {   //!< 自检签名指纹
    0x17, 0xbb, 0xc2, 0xcc, 0x7e, 0x8a, 0x30, 0xf8,
    0x2c, 0x46, 0xed, 0x69, 0x3a, 0x9d, 0x27, 0xeb,
    0xd1, 0x3b, 0x72, 0xfe, 0xdc, 0x30, 0xde, 0xa9,
    0xbc, 0xc9, 0x90, 0xe1, 0xce, 0x96, 0xe5, 0x06,
    0xf0, 0x1a, 0x29, 0xa7, 0x1b, 0xc6, 0xa3, 0xe2,
    0x7b, 0x89, 0xcc, 0x01, 0x0d, 0x08, 0x6e, 0x11,
    0x28, 0xfd, 0x42, 0x3c, 0xaf, 0xc7, 0x60, 0xbb,
    0x94, 0xdf, 0x85, 0x42, 0x70, 0xc2, 0xc4, 0x3f,
};

static unsigned char s_sm3_pt1[] = {    //!< SM3明文1
    "abd",
};
static unsigned char s_sm3_digest1[] = {    //!< SM3明文1摘要
    0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
    0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
    0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
    0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0,
};
static unsigned char s_sm3_pt2[] = {    //!< SM3明文2
    0x2b, 0xfa, 0x5f, 0x60, 0x5e, 0x63, 0x6b, 0x4b,
    0x06, 0x9a, 0xe2, 0xe2, 0x61, 0x44, 0xd5, 0xbb,
    0x4a, 0xc3, 0xcf, 0x08, 0x09, 0xcc, 0xd9, 0x5e,
    0x3f, 0xb0, 0xa6, 0xae, 0xea, 0xe2, 0xc4, 0xfb,
    0x71, 0x03, 0x4d, 0x71, 0xdd, 0xb0, 0xa2, 0x6a,
    0xac, 0x53, 0x53, 0x87, 0x30, 0x62, 0x9e, 0xcb,
    0x14, 0xd5, 0xc7, 0xf6, 0x58, 0x9f, 0x7d, 0x5c,
    0x3b, 0x20, 0xa7, 0xd8, 0x6d, 0x97, 0xa9, 0x15,
    0x00,
};
static unsigned char s_sm3_digest2[] = {    //!< SM3明文2摘要
    0x76, 0x77, 0x31, 0xE5, 0x3D, 0x62, 0x5F, 0x45,
    0x1B, 0xED, 0xEB, 0xCA, 0x09, 0xC1, 0xE5, 0x29,
    0xA7, 0xED, 0xB3, 0x1F, 0xA0, 0x4D, 0xD0, 0x97,
    0xBD, 0x48, 0x0B, 0xE1, 0x28, 0xAE, 0x89, 0x34,
};

static unsigned char s_sm4_key[] = {    //!< SM4密钥
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};
static unsigned char s_sm4_pt[] = { //!< SM4明文
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};
static unsigned char s_sm4_ct[] = { //!< SM4密文
    0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
    0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
};
static unsigned char s_sm4_cardno[] = { //!< 卡号: 999012345678
    0x39, 0x39, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
};
static unsigned char s_sm4_pin[] = {    //!< PIN: 9876
    0x39, 0x38, 0x37, 0x36,
};
static unsigned char s_sm4_padding[] = {    //!< padding, random datas
    0x25, 0x37, 0x93, 0x84, 0xE4, 0x54, 0xD1, 0x74,
};
static unsigned char s_sm4_cp[] = { //!< Encrypted PIN
    0xB9, 0xDB, 0x31, 0xB3, 0xFE, 0x32, 0xF2, 0x68,
    0x88, 0x26, 0xEA, 0xC8, 0xE7, 0x12, 0x48, 0x7F
};

#if ((defined(CFG_SM2) && defined(CFG_SM3))\
        || (defined(CFG_SM2_H) || defined(CFG_SM3_H)))
static int sm2_keygen_st(void)
{
    dbg_out_H(DS_SM_ST, "SM2: Generate key pair");
    int ret;

    // 生成公私�?
    dbg_out(DS_SM_ST, "\r\n Generate private & public key...");
    unsigned char prik_t[0x20] = { 0 };
    unsigned char pubk_t[0x40] = { 0 };
    ret = sm2_generate_pubk(NULL, TYPE_GFp, 256,
            prik_t, pubk_t, pubk_t + 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return ret;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get private key:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)prik_t, sizeof(prik_t));
    dbg_out_I(DS_SM_ST_DAT, " * Get public key:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pubk_t, sizeof(pubk_t));
    dbg_out(DS_SM_ST, "OK");

    // 使用公钥加密特定数据
    dbg_out(DS_SM_ST, "\r\n Encrypt fixed plain text by public key above...");
    unsigned char ct_enc[40 + 0x60];
    memset(ct_enc, 0x00, sizeof(ct_enc));
    ret = sm2_encrypt(NULL, TYPE_GFp, 256,
            s_sm2_pt, ct_enc, pubk_t, pubk_t + 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get encrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)ct_enc, strlen((char *)s_sm2_pt) + 0x60);
    dbg_out(DS_SM_ST, "OK");

    // 用私钥解�?
    dbg_out(DS_SM_ST, "\r\n Decrypt encrypted text by private key above...");
    unsigned char pt_dec[40 + 0x60];    // 软解时需要加大空间防止溢�?BUG)
    memset(pt_dec, 0x00, sizeof(pt_dec));
    ret = sm2_decrypt(NULL, TYPE_GFp, 256,
            prik_t, ct_enc, strlen((char *)s_sm2_pt) + 0x60, pt_dec);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get decrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pt_dec, sizeof(pt_dec) - 0x60);
    dbg_out(DS_SM_ST, "OK");

    // 对比
    dbg_out(DS_SM_ST, "\r\n Check decrypted text...");
    if(memcmp(pt_dec, s_sm2_pt, strlen((char *)s_sm2_pt))) {
        dbg_out_E(DS_SM_ST, "Decrypted text error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm2_enc_st(void)
{
    dbg_out_H(DS_SM_ST, "SM2: Encrypt");
    unsigned char pt_dec[40 + 0x60];    // 软解时需要加大空间防止溢�?BUG)
    int ret;

    // 使用SM2私钥对预置密文进行解密得到明�?
    dbg_out(DS_SM_ST, "\r\n Decrypt fixed cypher text...");
    memset(pt_dec, 0x00, sizeof(pt_dec));
    ret = sm2_decrypt(NULL, TYPE_GFp, 256,
            s_sm2_prik, s_sm2_ct, sizeof(s_sm2_ct), pt_dec);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get decrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pt_dec, sizeof(pt_dec) - 0x60);
    dbg_out(DS_SM_ST, "OK");
    // 比较解密明文与预置明文是否一�?
    dbg_out(DS_SM_ST, "\r\n Check decrypted text...");
    if(memcmp(pt_dec, s_sm2_pt, strlen((char *)s_sm2_pt))) {
        dbg_out_E(DS_SM_ST, "Decrypted text error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    // 使用SM2公钥加密预置明文得到密文
    unsigned char ct_enc[40 + 0x60];
    dbg_out(DS_SM_ST, "\r\n Encrypt fixed plain text...");
    memset(ct_enc, 0x00, sizeof(ct_enc));
    ret = sm2_encrypt(NULL, TYPE_GFp, 256,
            s_sm2_pt, ct_enc, s_sm2_pubk, s_sm2_pubk + 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get encrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)ct_enc, strlen((char *)s_sm2_pt) + 0x60);
    dbg_out(DS_SM_ST, "OK");
    // 再用SM2私钥解密所得密文得到明�?
    dbg_out(DS_SM_ST, "\r\n Decrypt encrypted text...");
    memset(pt_dec, 0x00, sizeof(pt_dec));
    ret = sm2_decrypt(NULL, TYPE_GFp, 256,
            s_sm2_prik, ct_enc, strlen((char *)s_sm2_pt) + 0x60, pt_dec);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get decrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pt_dec, sizeof(pt_dec) - 0x60);
    dbg_out(DS_SM_ST, "OK");
    // 比对明文与预置明文是否一�?
    dbg_out(DS_SM_ST, "\r\n Check decrypted text...");
    if(memcmp(pt_dec, s_sm2_pt, strlen((char *)s_sm2_pt))) {
        dbg_out_E(DS_SM_ST, "Decrypted text error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm2_dec_st(void)
{
    dbg_out_H(DS_SM_ST, "SM2: Decrypt");
    unsigned char pt_dec[40 + 0x60];    // 软解时需要加大空间防止溢�?BUG)
    int ret;

    // 使用SM2私钥对预置密文进行解密得到明�?
    dbg_out(DS_SM_ST, "\r\n Decrypt fixed cypher text...");
    memset(pt_dec, 0x00, sizeof(pt_dec));
    ret = sm2_decrypt(NULL, TYPE_GFp, 256,
            s_sm2_prik, s_sm2_ct, sizeof(s_sm2_ct), pt_dec);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Error! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get decrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pt_dec, sizeof(pt_dec) - 0x60);
    dbg_out(DS_SM_ST, "OK");
    // 比较解密明文与预置明文是否一�?
    dbg_out(DS_SM_ST, "\r\n Check decrypted text...");
    if(memcmp(pt_dec, s_sm2_pt, sizeof(s_sm2_pt))) {
        dbg_out_E(DS_SM_ST, "Decrypted text error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm2_sign_st(void)
{
    dbg_out_H(DS_SM_ST, "SM2: Sign");
    int ret;
    int len;
    unsigned char pt_t[128];

    // 使用SM2对数字签名进行验�?
    dbg_out(DS_SM_ST, "\r\n Verify fixed signature...");
    len = strlen((char *)s_sm2_pt);
    memmove(pt_t, s_sm2_pt, len);
    memmove(pt_t + len, s_sm2_sign, sizeof(s_sm2_sign));
    len += sizeof(s_sm2_sign);
    ret = sm2_verify(pt_t, len, s_sm2_pubk, s_sm2_id, 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Verify failed! %d", ret);
        return ret;
    }
    dbg_out(DS_SM_ST, "OK");

    // 使用SM2私钥对代签名信息进行签名
    unsigned char sign_t[64] = { 0 };
    unsigned char pubk_t[64] = { 0 };
    dbg_out(DS_SM_ST, "\r\n Sign fixed plain text...");
    ret = sm2_sign(s_sm2_pt, strlen((char *)s_sm2_pt), s_sm2_id,
            strlen((char *)s_sm2_id), s_sm2_prik, sign_t, pubk_t);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Sign failed! %d", ret);
        return ret;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get signature:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)sign_t, sizeof(sign_t));
    dbg_out(DS_SM_ST, "OK");

    // 用SM2公钥验签
    dbg_out(DS_SM_ST, "\r\n Verify above signature...");
    len = strlen((char *)s_sm2_pt);
    memmove(pt_t, s_sm2_pt, len);
    memmove(pt_t + len, sign_t, sizeof(sign_t));
    len += sizeof(sign_t);
    ret = sm2_verify(pt_t, len, s_sm2_pubk, s_sm2_id, 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Verify failed! %d", ret);
        return ret;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm2_verify_st(void)
{
    dbg_out_H(DS_SM_ST, "SM2: Verify");

    // 使用SM2对数字签名进行验�?
    dbg_out(DS_SM_ST, "\r\n Verify fixed signature...");
    unsigned char pt_t[128];
    int len = strlen((char *)s_sm2_pt);
    memmove(pt_t, s_sm2_pt, len);
    memmove(pt_t + len, s_sm2_sign, sizeof(s_sm2_sign));
    len += sizeof(s_sm2_sign);
    int ret = sm2_verify(pt_t, len, s_sm2_pubk, s_sm2_id, 0x20);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "Verify failed! %d", ret);
        return ret;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm2_st(void)
{
    dbg_out_I(DS_SM_ST_DAT, " * Fixed private key:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_prik, sizeof(s_sm2_prik));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed public key:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_pubk, sizeof(s_sm2_pubk));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed plain text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_pt, sizeof(s_sm2_pt));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed cypher text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_ct, sizeof(s_sm2_ct));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed ID:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_id, sizeof(s_sm2_id));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed signature:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm2_sign, sizeof(s_sm2_sign));

    if(sm2_keygen_st()) {
        return SMST_RET_SM2_KEYGEN_ERR;
    }
    if(sm2_enc_st()) {
        return SMST_RET_SM2_ENC_ERR;
    }
    if(sm2_dec_st()) {
        return SMST_RET_SM2_DEC_ERR;
    }
    if(sm2_sign_st()) {
        return SMST_RET_SM2_SIGN_ERR;
    }
    if(sm2_verify_st()) {
        return SMST_RET_SM2_VERIFY_ERR;
    }
    return 0;
}
#endif /* ((defined(CFG_SM2) && defined(CFG_SM3))\
          || (defined(CFG_SM2_H) || defined(CFG_SM3_H))) */
#if (defined(CFG_SM3) || defined(CFG_SM3_H))
static int sm3_st(void)
{
    dbg_out_I(DS_SM_ST_DAT, " * Fixed plain text1:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm3_pt1, sizeof(s_sm3_pt1));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed plain text2:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm3_pt2, sizeof(s_sm3_pt2));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed digest1:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm3_digest1, sizeof(s_sm3_digest1));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed digest2:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm3_digest2, sizeof(s_sm3_digest2));
    unsigned char digest[32];

    dbg_out_H(DS_SM_ST, "SM3: Digest 1");
    // 获取摘要
    dbg_out(DS_SM_ST, "\r\n Get plain text 1 digest...");
    sm3(s_sm3_pt1, strlen((char *)s_sm3_pt1), digest);
    dbg_out_I(DS_SM_ST_DAT, " * Get plain text 1 digest:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)digest, sizeof(digest));
    dbg_out(DS_SM_ST, "OK");
    // 对比摘要
    dbg_out(DS_SM_ST, "\r\n Check digest");
    if(memcmp(digest, digest, sizeof(digest))) {
        dbg_out_E(DS_SM_ST, "Digest check error!");
        return SMST_RET_SM3_ERR;
    }
    dbg_out(DS_SM_ST, "OK");

    dbg_out_H(DS_SM_ST, "SM3: Digest 2");
    // 获取摘要
    dbg_out(DS_SM_ST, "\r\n Get plain text 2 digest...");
    sm3(s_sm3_pt2, strlen((char *)s_sm3_pt2), digest);
    dbg_out_I(DS_SM_ST_DAT, " * Get plain text 2 digest:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)digest, sizeof(digest));
    dbg_out(DS_SM_ST, "OK");
    // 对比摘要
    dbg_out(DS_SM_ST, "\r\n Check digest");
    if(memcmp(s_sm3_digest2, digest, sizeof(digest))) {
        dbg_out_E(DS_SM_ST, "Digest check error!");
        return SMST_RET_SM3_ERR;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
#endif /* (defined(CFG_SM3) || defined(CFG_SM3_H)) */
#if (defined(CFG_SM4) || defined(CFG_SM4_H))
static int sm4_enc_st(void)
{
    dbg_out_H(DS_SM_ST, "SM4: Encrypt");
    TRACE("\r\nSM4 Encrypt");
    int ret;
    unsigned char ct[32];

    // 加密明文得到密文
    dbg_out(DS_SM_ST, "\r\n Encrypt fixed plain text...");
    memset(ct, 0x00, sizeof(ct));
    
    TRACE_BUF("sm4 pt",s_sm4_pt,sizeof(s_sm4_pt));
    ret = sm4_encrypt(s_sm4_pt, sizeof(s_sm4_pt), s_sm4_key, ct);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "SM4 Encrypt failed! %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get encrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)ct, sizeof(s_sm4_pt));
    dbg_out(DS_SM_ST, "OK");
    
    TRACE_BUF("sm4 ct",ct,sizeof(ct));
    TRACE_BUF("sm4 ct 预设",s_sm4_ct,sizeof(s_sm4_ct));
    
    // 比较生产密文与预置密文是否一�?
    dbg_out(DS_SM_ST, "\r\n Check encrypted text above...");
    if(memcmp(ct, s_sm4_ct, sizeof(s_sm4_ct))) {
        dbg_out_E(DS_SM_ST, "Encrypted text check error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm4_dec_st(void)
{
    dbg_out_H(DS_SM_ST, "SM4: Decrypt");
    int ret;
    unsigned char pt[32];
    
    TRACE("\r\nsm4 Decrypt");

    // 加密明文得到密文
    dbg_out(DS_SM_ST, "\r\n Decrypt fixed cipher text...");
    memset(pt, 0x00, sizeof(pt));
    ret = sm4_decrypt(s_sm4_ct, sizeof(s_sm4_ct), s_sm4_key, pt);
    if(ret < 0) {
        dbg_out_E(DS_SM_ST, "SM4 Encrypt failed! %d", ret);
        return -1;
    }
    
    TRACE_BUF("sm4 pt",pt,sizeof(pt));
    
    dbg_out_I(DS_SM_ST_DAT, " * Get decrypted text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)pt, sizeof(s_sm4_pt));
    dbg_out(DS_SM_ST, "OK");

    // 比较生产密文与预置密文是否一�?
    dbg_out(DS_SM_ST, "\r\n Check decrypted text above...");
    if(memcmp(pt, s_sm4_pt, sizeof(s_sm4_pt))) {
        dbg_out_E(DS_SM_ST, "Decrypted text check error!");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm4_pinblock_st(void)
{
//    dbg_out_H(DS_SM_ST, "SM4: PIN Block");
//    unsigned char ct[64] = { 0 };
//    unsigned char tmp[32] = { 0 };
//
//    dbg_out(DS_SM_ST, "\r\n Encrypt PIN...");
//    extern int x98_pinblock_format0(const uint8_t cardno[12],
//            const uint8_t *pin, uint8_t pin_len, uint8_t output[8]);
//    int ret;
//    int len = 8;
//    ret = x98_pinblock_format0(s_sm4_cardno, s_sm4_pin, sizeof(s_sm4_pin), tmp);
//    if(ret) {
//        dbg_out_E(DS_SM_ST, "PIN block analyze failed, return %d", ret);
//        return -1;
//    }
//    memmove(tmp + len, s_sm4_padding, sizeof(s_sm4_padding));
//    len += sizeof(s_sm4_padding);
//    ret = sm4_encrypt(tmp, len, s_sm4_key, ct);
//    if(ret) {
//        dbg_out_E(DS_SM_ST, "SM4 Encrypt failed! %d", ret);
//        return -1;
//    }
//    dbg_out_I(DS_SM_ST_DAT, " * Get encrypted PIN:");
//    dbg_dmp_HC(DS_SM_ST_DAT, (char *)ct, len);
//    dbg_out(DS_SM_ST, "OK");
//
//    // 判断加密是否正确
//    dbg_out(DS_SM_ST, "\r\n Check decrypted PIN above...");
//    if(memcmp(ct, s_sm4_cp, sizeof(s_sm4_cp))) {
//        dbg_out_E(DS_SM_ST, "Encrypted PIN check error!");
//        return -1;
//    }
//    dbg_out(DS_SM_ST, "OK");

    return 0;
}
static int sm4_st(void)
{
    dbg_out_I(DS_SM_ST_DAT, " * Fixed key:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_key, sizeof(s_sm4_key));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed plain text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_pt, sizeof(s_sm4_pt));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed cipher text:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_ct, sizeof(s_sm4_ct));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed Card NO.:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_cardno, sizeof(s_sm4_cardno));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed PIN:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_pin, sizeof(s_sm4_pin));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed padding:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_padding, sizeof(s_sm4_padding));
    dbg_out_I(DS_SM_ST_DAT, " * Fixed cipher PIN:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)s_sm4_cp, sizeof(s_sm4_cp));

    if(sm4_enc_st()) {
        return SMST_RET_SM4_ENC_ERR;
    }
    if(sm4_dec_st()) {
        return SMST_RET_SM4_DEC_ERR;
    }
    if(sm4_pinblock_st()) {
        return SMST_RET_SM4_PINBLOCK_ERR;
    }

    return 0;
}
#endif /* (defined(CFG_SM4) || defined(CFG_SM4_H)) */

/** { 扑克检验说明：
 * \detail 
 *      扑克检验：
 *      1)  介绍�?
 *              序列长度为n，对任意的正整数m，长度为m位的二进制位序列�?^m�?
 *              可能性。将待检序列划分成``k = n / m``个长度为m的非叠加�?
 *              子序列，用n_i(0 <= i <= 2^m - 1)表示第i种子序列类型的个数�?
 *              检测目的：检测这2^m种子序列类型的个数是否相等�?
 *              如果序列未通过扑克检验，说明有某个或某几个子序列类型的个数过�?
 *              或过少，序列不符合随机性要求。序列长度n和子序列长度m的取值必�?
 *              满足不等式``k = (n / m) >= 5 * 2^m``。当m=1时，则扑克检验等价于
 *              频数检验�?
 *      2)  针对特性：
 *              均衡性、游程性、不可预测性�?
 *      3)  特性证�?
 *              均衡性：
 *                  假若某一序列中元素全部为0或为1。则n_0或n_15=5000。其�?
 *                  15种状态均没有出现，无法谈�?6种状态是否均匀分布。所�?
 *                  该序列既不满足均衡性，又不能通过扑克检验。所以如果某一
 *                  序列的均衡性不是很好，就不一定能通过扑克检验，从而证�?
 *                  扑克检验所针对特性有均衡性�?
 *              游程性：
 *                  假若某一序列是有0�?交替组成的，即序列中的游程全部为
 *                  长度1�?�?的游程，形如0101...�?010...，则n_5_0101�?
 *                  n_5_1010=5000。其�?5种状态均没有出现，依然无法谈�?6
 *                  种状态是否均匀分布。所以该序列既不满足游程性，又不�?
 *                  通过扑克检验，从而证明扑克检验所针对的特性有游程性�?
 *              不可预测性：
 *                  假若某一序列中有一规律，即出现一�?后，紧接着会出�?
 *                  一�?。则将此序列分组后的四位二元码分为三类：01XX�?
 *                  X01X，XX01，其中X代表0�?。在该情况下，当一一列举16�?
 *                  状态的二元码时，可以发�?000�?000�?001�?100没有出现
 *                  过。则可知�?6种状态并未随机均匀分布。所以如果某一序列
 *                  的不可预测性不是很好，也就是说序列之间元素有一定相关�?
 *                  时，该序列就不一定能通过扑克检验，从而证明扑克检验所
 *                  针对的特性有不可预测性�?
 *
 }*/
static int random_poker_test(int nByteLen, unsigned char * pRandom)
{
    int i, j;
    int dblByteLen = nByteLen << 1;
    float SCnt[16] = { 0 };
    float SumSqrt = 0;
    float X = 0;

    if(nByteLen > 1250) {
        return 0;
    }
    for(i = 0; i < nByteLen; i++) {
        int h = (pRandom[i] >> 4) & 0x0f;
        int l = pRandom[i] & 0x0f;
        SCnt[h]++;
        SCnt[l]++;
    }
    for(j = 0; j < 16; j++) {
        SumSqrt += (SCnt[j] * SCnt[j]);//pow(SCnt[j], 2);
    }
    X = 16.0;
    X /= (float)dblByteLen;
    X *= SumSqrt;
    X -= (float)dblByteLen;
    dbg_out(DS_SM_ST, "(%3.6f) ", X);
    if(X <= 2.16 && X >= 46.17) {
        return -1;
    }

    return 0;
}
static int rand_test(int size)
{
    unsigned char rand[4096] = { 0 };
    int ret;

    dbg_out(DS_SM_ST, "\r\n Get random data %d...", size);
    ret = sc_is8u_get_random_data(size, rand);
    if(ret != size) {
        dbg_out_E(DS_SM_ST, "Error: %d", ret);
        return -1;
    }
    dbg_out_I(DS_SM_ST_DAT, " * Get random data:");
    dbg_dmp_HC(DS_SM_ST_DAT, (char *)rand, size);
    dbg_out(DS_SM_ST, "test...");
    if(random_poker_test(size, rand)) {
        dbg_out_E(DS_SM_ST, "Error");
        return -1;
    }
    dbg_out(DS_SM_ST, "OK");

    return 0;
}

static int rand_st(void)
{
    dbg_out_H(DS_SM_ST, "Random test");
    int i;

    for(i = 0; i < 20; i++) {
        if(rand_test(1250)) {
            return SMST_RET_RAND_ERR;
        }
    }

    return 0;
}

int sm_selftest(void)
{
    int ret = 0;
    unsigned int time = sys_get_counter();
    do {
        dbg_out_I(DS_SM_ST, "# IS8U256A Init...");
        if((ret = sc_is8u256a_init())) {
            break;
        }
#if ((defined(CFG_SM2) && defined(CFG_SM3))\
        || (defined(CFG_SM2_H) || defined(CFG_SM3_H)))
        dbg_out_I(DS_SM_ST, "# SM2 self test...");
        if((ret = sm2_st())) {
            break;
        }
#endif /* ((defined(CFG_SM2) && defined(CFG_SM3))\
          || (defined(CFG_SM2_H) || defined(CFG_SM3_H))) */
#if (defined(CFG_SM3) || defined(CFG_SM3_H))
        dbg_out_I(DS_SM_ST, "# SM3 self test...");
        if((ret = sm3_st())) {
            break;
        }
#endif /* (defined(CFG_SM3) || defined(CFG_SM3_H)) */
#if (defined(CFG_SM4) || defined(CFG_SM4_H))
        dbg_out_I(DS_SM_ST, "# SM4 self test...");
        if((ret = sm4_st())) {
            break;
        }
#endif /* (defined(CFG_SM4) || defined(CFG_SM4_H)) */
        dbg_out_I(DS_SM_ST, "# Random self test...");
        if((rand_st())) {
            break;
        }
    } while(0);
        time = sys_get_counter() - time;
        dbg_out_I(DS_SM_ST, "# Used time: %d ms", time);
        return ret;
}
#endif /* defined(DEBUG_Dx) */
#endif /* defined(CFG_SECURITY_CHIP) */

