#ifndef _FILE_H_
#define _FILE_H_

//  返回值定义
#define     FS_INVAL            3901
#define     FS_EXIST            3902
#define     FS_NOTEXIST         3903
#define     FS_OPENED           3904
#define     FS_NOTOPENED        3905
#define     FS_NOSPACE          3906
#define     FS_MAXFILENO        3907

//  文件打开模式，可使用"或"组合
#ifndef O_RDWR
#define     O_RDWR              02
#endif
#ifndef O_CREAT
#define     O_CREAT             0100
#endif
//  文件定位起点
#ifndef SEEK_SET
#define     SEEK_SET            0
#endif
#ifndef SEEK_CUR
#define     SEEK_CUR            1
#endif
#ifndef SEEK_END
#define     SEEK_END            2
#endif
//  文件属性
#define     FILE_BELONG_KERNEL      0xFF    //  文件属于kernel
#define     FILE_BELONG_DELETE      0xFE    //  文件被删除
//  文件类型
#define     FILE_TYPE_BOOTLOADER    0x80    //  BOOT LOADER软件
#define     FILE_TYPE_BOOT          0x81    //  BOOT软件
#define     FILE_TYPE_CTRL          0x82    //  CTRL软件
#define     FILE_TYPE_FONT          0xA0    //  字库文件

#define     FILE_TYPE_APP           0x01    //  应用程序
#define     FILE_TYPE_DATA          0x03    //  应用程序的数据文件
#define     FILE_PINPAD_APP         0x04    //  应用程序对应的密码键盘程序

typedef struct
{
	int8_t    file_name[16];
	uint16_t  belong_app;
	uint16_t  StartBlock;
	int32_t   size;
}FILE_ITEM;

//typedef struct
//{
//    char    filename[32];
//    int     belong_app;
//    int     file_len;
//} file_attr_t;


// 系统内部函数
void   fs_init(void);
void   fs_format(void);

// 提供给用户使用的API函数
int fs_open(const char *file_name, int open_mode);
int fs_close(int fileno);
int fs_seek(int fileno, int offset, int whence);
int fs_read(int fileno, void *outbuf, int bytelen);
int fs_write(int fileno, const void *inbuf, int bytelen);
int fs_size(const char *file_name);
int fs_exist(const char *file_name);
int fs_truncate(int fileno);
int fs_remove(const char *file_name);
int fs_rename(const char *oldname, const char *newname);
int fs_freespace(void);
int fs_list(int begno, file_attr_t *fileattr);

#endif

