#ifndef _FILEDEF_H_
#define _FILEDEF_H_

//#define FILE_TOTOAL_SIZE    (1024*1024)                         // 文件系统占用空间1M
#define FILE_TOTOAL_SIZE    (512*1024)      
#define FS_SECTOR_SIZE   0x1000                            // 1个扇区大小
#define MAX_FILES           2                               // 最大文件数
#define FS_UNIT_LEN         0x1000                            // 1个基本存储单元块4096
#define SECTOR_BLOCK        (FS_SECTOR_SIZE / FS_UNIT_LEN) 		// 每个数据扇区包含的数据块数 
#define LOG_SIZE            (FS_SECTOR_SIZE - 40)					            // 日志大小
#define DATA_SECTORS_MAX    ((FILE_TOTOAL_SIZE / FS_SECTOR_SIZE) - 3)
#define ALL_BLOCKS          (DATA_SECTORS_MAX * SECTOR_BLOCK)
#define FAT_SIZE            (MAX_FILES * sizeof(FILE_ITEM) + ALL_BLOCKS*2 + 32)

#define FAT_SECTOR_SIZE     (FS_SECTOR_SIZE*1)// FAT表所在扇区的大小
#define LOG_SECTOR_SIZE     (FS_SECTOR_SIZE*1)// 日志表所在扇区的大小
#define	DATA_SECTOR_SIZE    (FS_SECTOR_SIZE*1)// 数据扇区的大小
#define FAT1_SECTORS        ((FAT_SIZE / FAT_SECTOR_SIZE) + !!(FAT_SIZE % FAT_SECTOR_SIZE))
#define FAT2_SECTORS        FAT1_SECTORS
#define LOG_SECTORS         1

#define FILE_BASE_ADDR      0//                                          // FLASH内存基地址
#define	FAT1_ADDR	        (FILE_BASE_ADDR + 0x0000)	                    // FAT1表的地址
#define	FAT2_ADDR	        (FAT1_ADDR + FAT_SECTOR_SIZE * FAT1_SECTORS)	// FAT2表的地址
#define	LOG_ADDR	        (FAT2_ADDR + FAT_SECTOR_SIZE * FAT2_SECTORS)   // 日志表的地址
#define	DATA_ADDR	        (LOG_ADDR + LOG_SECTOR_SIZE * LOG_SECTORS)     // 数据扇区的起始地址

#define DATA_SECTORS        ((FILE_TOTOAL_SIZE / FS_SECTOR_SIZE) - (FAT1_SECTORS + FAT2_SECTORS + LOG_SECTORS))


//注释：
//如果FAT扇区太小,空间不够的情况下,MAX_FILES可能会被系统重定义,
//所以实际的最大文件数可能小于你的定义值

// 函数定义
void s_fsprompt_begformat(void);
void s_fsprompt_erase(uint32_t Addr);
void s_fsprompt_percent(uint16_t per);

#endif

