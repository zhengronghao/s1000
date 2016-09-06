#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define     BT_VAL                4301        //  无效的通讯参数
#define     BT_NOTOPEN            4302        //  蓝牙串口未打开
#define     BT_BUSY               4303        //  蓝牙串口忙
#define     BT_ERROR              4304        //  蓝牙错误
#define     BT_NOTEXIST           4305        //  蓝牙模块不存在
#define     BT_NOTREADY           4306        //  未连接
#define     BT_NOTCONNECT         4307        // 连接未建立，不能发送数据

#define BT_IOCTL_SET_NAME       1
#define BT_IOCTL_GET_NAME       2
#define BT_IOCTL_GET_PIN        3
#define BT_IOCTL_SET_PIN        4
#define BT_IOCTL_GET_BTADDR     5
#define BT_IOCTL_CHECK_CONNECTED 6

union bt_ioctl_arg
{
    char name[32];
    char pin[32];
    unsigned char addr[6];
};

int check_if_online(void);
int bt_open(void);
int bt_close(void);
int bt_read(void *outbuf, int bytelen, int timeOut_ms);
int bt_write(const void *inbuf, int bytelen);
#define bt_printf(format,...) bt_Printf(format,##__VA_ARGS__)
//int bt_printf(const char *format, ...);
int bt_check_readbuf(void);
int bt_check_writebuf(void);
int bt_clear(void);
int bt_setname(uchar *pucBuff, int len);
int bt_setpin(uchar *pucBuff, int len);
int bt_ioctl(int cmd, union bt_ioctl_arg *arg);
int bt_ready(void);
uint8_t bt_status(void);
int bt_getname(char *name);
int bt_getpin(char *pin);
int bt_getaddr(unsigned char *addr);
int fac_boardtest_bt(int mode);


#ifdef __cplusplus
}
#endif

#endif  /*__BLUETOOTH_H__*/

