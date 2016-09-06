/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : file.c
 * bfief              : 
 * Author             : yehf()  
 * Email              : yehf@itep.com.cn
 * Version            : V0.00
 * Date               : 
 * Description        : 1.只支持2个文件，不支持文件名修改 
 *******************************************************************************/
#include "wp30_ctrl.h"
#ifdef CFG_FS
#define     USED_BLOCK          0xff00
#define     NULL_BLOCK          0xffff

#define     FILE_CMD_OK         0x0000
#define     FILE_CMD_SET        0x55AA

#define     LOG_CREAT_FILE          0xFFFE
#define     LOG_WRITE_FILE          0xFFFD
#define     LOG_REMOVE_FILE         0xFFFB
#define     LOG_TRUNCATE_FILE       0xFFF7
#define     LOG_EXCHANGE_DATA       0xFFEF
#define     LOG_RENAME_FILE         0xFFDF
#define     LOG_FWRITE_FAILED       0x7FFF

#define     LOG_END_ADDR        (uint16_t *)(LOG_ADDR + LOG_SIZE)
#define     BLOCK_SIZE          (DATA_SECTOR_SIZE/SECTOR_BLOCK)
#define     FAT1_LOG_ADDR       (LOG_ADDR + 2)
#define     FAT2_LOG_ADDR       (LOG_ADDR + 4)
#define     INIT_LOG_ADDR       (LOG_ADDR + 6)

#define     FAT_FLAG_VER        "FS:01.01"
#define     LOG_FLAG_VER        "FL:01.01"
#define     FORMAT_FLAG         "FORMATED"

/* #define SRC_DEC_SPACE_CONFLIC */


typedef struct
{
	uint16_t OpenFlag;
	uint16_t fd;
	int32_t  FilePtr;
	int32_t  len;
	uint16_t UpBlockPtr;
	uint16_t CurBlockPtr;
	uint16_t FilePtrOff;
	uint16_t CmdSet;
}FILE_STATUS;

/* extern uint32_t		DATA_SECTORS; */

/* static uint32_t		FAT_SIZE; */
/* static uint32_t		ALL_BLOCKS; */

// 内存占用: 2*6+4+258*2+28*2+20*2+4+4=636B
static uint16_t		k_FreeBlockNum         ;
static uint16_t		k_UsedBlockNum         ;
static uint16_t		k_fsRecover            ;
static uint16_t		k_FreeBlockPtr         ;
static uint16_t		vCMD_OK                ;
static uint16_t		vCMD_SET               ;
static uint32_t		k_ExchangeAddr         ;
static uint16_t		k_FAT[ALL_BLOCKS + 2]  ;  //258*2
static FILE_ITEM	k_Flist[MAX_FILES]     ;  //28*2
static FILE_STATUS	k_Fstatus[MAX_FILES]   ;  //20*2
static uint16_t		*k_LogPtr              ;
static uint16_t		*k_FailLogPtr          ;
//uint8_t *k_filetptr = NULL;


int _flash_readb(void *des, uint32_t src)
{
    hw_w25x_flash_read((uint8_t *)des,(uint32_t)src,1);
    return 0;
}
#define __flash_readb(w_addr, addr)  _flash_readb(w_addr, (uint32_t)(addr))
int _flash_readw(void *des, uint32_t src)
{
    hw_w25x_flash_read((uint8_t *)des,(uint32_t)src,2);
    return 0;
}
#define __flash_readw(w_addr, addr)  _flash_readw(w_addr, (uint32_t)(addr))
int _flash_readl(void *des, uint32_t src)
{
    hw_w25x_flash_read((uint8_t *)des,(uint32_t)src,4);
    return 0;
}
#define __flash_readl(w_addr, addr)  _flash_readl(w_addr, (uint32_t)(addr))
int _flash_read(void *des, uint32_t src, uint32_t size_byte)
{
    hw_w25x_flash_read((uint8_t *)des,src,(uint16_t)size_byte);
    return 0;
}
#define __flash_read(des, src, size_byte)  _flash_read(des, (uint32_t)(src),size_byte)
int __flash_erase(uint32_t addr)
{
    hw_w25_flash_erase_sector(addr);
    return 0;
}
int __flash_write(uint16_t *addr, void *buf, uint32_t len)
{
    //spiflash按8位操作所以数据长度*2
    hw_w25x_flash_write((uint8_t *)buf,(uint32_t)addr,(uint16_t)len*2);
    return 0;
}
int __flash_writeEx(uint16_t *des, uint16_t *src, uint32_t len)
{
    uint8_t *k_filetptr = NULL;
    k_filetptr = malloc(BLOCK_SIZE);
    if ( k_filetptr == NULL ) {
        return 1;
    }
    __flash_read(k_filetptr, (uint32_t)src, FAT_SIZE);
    //spiflash按8位操作所以数据长度*2
    __flash_write(des, k_filetptr, (uint16_t)len*2);
    free(k_filetptr);
    return 0;
}
int __flash_write_failed(void)
{
    return 0;
}
int s_fname_convert(const char *fullname, char *shortname, int *belong_app)
{
    //int         fd;
    int temp;
    //char        *p;
    const char  *pname;
    char        FName[17];

    if(fullname == NULL || strlen(fullname) == 0)
    {
        return(-FS_INVAL);
    }
    pname = fullname;
    while(1)
    {
        if(*pname == '\t' || *pname == ' ')
        {
            pname++;
        }
        else
        {
            break;
        }
    }

    if (strlen(pname) > 16)
    {
        return(-FS_INVAL);
    }

    temp = UpperCase((char*)pname, FName, 16);
    FName[temp] = 0;

    if(temp <= 0)
    {
        return(-FS_INVAL);
    }

    if(shortname != NULL)
    {
        strcpy(shortname, FName);
    }

    if(belong_app != NULL)
    {
        *belong_app = 0xFF;
    }

    return(temp);
}

static void s_log_erase(void)
{
	uint16_t   flag[8];
    int32_t       j;

	memset(flag, 0, sizeof(flag));
	__flash_write((uint16_t *)(LOG_ADDR+LOG_SECTOR_SIZE-8), flag, 4);
	__flash_write((uint16_t *)LOG_ADDR, &vCMD_OK, 1);
    for (j = 0; j < LOG_SECTORS; j++)
    {
        __flash_erase(LOG_ADDR + j * LOG_SECTOR_SIZE);
    }
    memcpy(flag, LOG_FLAG_VER, strlen(LOG_FLAG_VER));
	__flash_write((uint16_t *)(LOG_ADDR+LOG_SECTOR_SIZE-8), flag, 4);
	k_LogPtr = (uint16_t *)(LOG_ADDR+16);
}

static void s_save_fat(void)
{
	int32_t       i;
    int32_t       j;
#ifdef SRC_DEC_SPACE_CONFLIC
	uint16_t      buff[1024];
#else
    uint16_t      buff[4];
#endif

	__flash_write((uint16_t *)FAT1_LOG_ADDR, &vCMD_SET, 1);

	for(i = 0; i < 3; i++)
	{
        for (j = 0; j < FAT1_SECTORS; j++)
        {
            __flash_erase(FAT1_ADDR + j * FAT_SECTOR_SIZE);
        }
        
		if(__flash_write((uint16_t *)(FAT1_ADDR+8), (uint16_t *)&k_ExchangeAddr, 2))
			continue;

		if(__flash_write((uint16_t *)(FAT1_ADDR+12), &k_FreeBlockNum, 1))
			continue;

		if(__flash_write((uint16_t *)(FAT1_ADDR+14), &k_UsedBlockNum, 1))
			continue;

		if(__flash_write((uint16_t *)(FAT1_ADDR+16), &k_FreeBlockPtr, 1))
			continue;

		if(__flash_write((uint16_t *)(FAT1_ADDR+18), k_FAT, ALL_BLOCKS))
			continue;

		if(__flash_write((uint16_t *)(FAT1_ADDR+ALL_BLOCKS*2+18), (uint16_t *)(&k_Flist[0]), (MAX_FILES*sizeof(FILE_ITEM))/2+1))
			continue;

		memcpy(buff,FAT_FLAG_VER,8);

		if(__flash_write((uint16_t *)(FAT1_ADDR), buff, 4))
			continue;

		__flash_write((uint16_t *)FAT1_LOG_ADDR, &vCMD_OK, 1);
		break;
	}

	if(i == 3)
	{
		__flash_write_failed();
	}

	for(i = 0; i < 3; i++)
	{
        int continue_flag;
#ifdef SRC_DEC_SPACE_CONFLIC
        unsigned int wr_size;
        uint16_t *src_addr;
        uint16_t *des_addr;
        unsigned int tmp_size;

        wr_size = (FAT_SIZE);
        src_addr = (uint16_t *)(FAT1_ADDR);
        des_addr = (uint16_t *)(FAT2_ADDR);
        continue_flag = 0;

        for (j = 0; j < FAT2_SECTORS; j++)
        {
            __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
        }
        while (wr_size)
        {
            if (wr_size > sizeof(buff))
            {
                tmp_size = sizeof(buff);
            }
            else
            {
                tmp_size = wr_size;
            }
            __flash_read(buff, (uint8_t *)src_addr, tmp_size);
            if(__flash_write(des_addr, buff, tmp_size / 2))
            {
                continue_flag = 1;
                break;
            }
            wr_size -= tmp_size;
            src_addr += (tmp_size / 2);
            des_addr += (tmp_size / 2);
        }
        if (continue_flag)
        {
            continue;
        }
#else
        continue_flag = 0;

        for (j = 0; j < FAT2_SECTORS; j++)
        {
            __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
        }

//        if(__flash_write((uint16_t *)(FAT2_ADDR), (uint16_t *)(FAT1_ADDR), FAT_SIZE/2))
        if(__flash_writeEx((uint16_t *)(FAT2_ADDR), (uint16_t *)(FAT1_ADDR), FAT_SIZE/2))
        {
            continue_flag = 1;
        }

        if (continue_flag)
        {
            continue;
        }
#endif
		__flash_write((uint16_t *)FAT2_LOG_ADDR, &vCMD_OK, 1);
		break;
	}

	if(i == 3)
	{
		__flash_write_failed();
	}
	s_log_erase();
}

static void s_log_ok(void)
{
	if(__flash_write(k_LogPtr, &vCMD_OK, 1))
	{
		s_save_fat();
	}
	else if(++k_LogPtr >= LOG_END_ADDR)
	{
		s_save_fat();
	}
}

static void s_log_truncate(uint16_t fd, int32_t len)
{
	uint16_t      buff[5];

	buff[0] = LOG_TRUNCATE_FILE;
	buff[1] = fd;
	buff[2] = *((uint16_t *)&len);
	buff[3] = *((uint16_t *)&len+1);
	buff[4] = FILE_CMD_SET;

	if(__flash_write(k_LogPtr, buff, 5))
	{
		s_save_fat();
		if(__flash_write(k_LogPtr, buff, 5))
		{
			__flash_write_failed();
		}
	}

	k_LogPtr += 4;

}

static void s_log_remove(uint16_t fd)
{
	uint16_t buff[3];

	buff[0] = LOG_REMOVE_FILE;
	buff[1] = fd;
	buff[2] = FILE_CMD_OK;
	if(__flash_write(k_LogPtr, buff, 3))
	{
		s_save_fat();
	}
	else
	{
		k_LogPtr += 3;
		if(k_LogPtr >= LOG_END_ADDR)
		{
			s_save_fat();
		}
	}
}

static void s_log_creat(uint16_t fd, int8_t *filename, int32_t belong_app)
{
	uint16_t buff[12];

	buff[0] = LOG_CREAT_FILE;
	buff[1] = fd;
    buff[2] = belong_app;
	memcpy(buff+3, filename, 16);
	buff[11] = FILE_CMD_OK;
	if(__flash_write(k_LogPtr, buff, 12))
	{
		s_save_fat();
	}
	else
	{
		k_LogPtr += 12;
		if(k_LogPtr >= LOG_END_ADDR)
		{
			s_save_fat();
		}
	}
}

static void s_log_rename(uint16_t fd, uint8_t *NewFileName, int32_t belong_app)
{
	uint16_t      buff[12];

	buff[0] = LOG_RENAME_FILE;
	buff[1] = fd;
    buff[2] = belong_app;
	memcpy(buff+3, NewFileName, 16);
	buff[11] = FILE_CMD_OK;
	if(__flash_write(k_LogPtr, buff, 12))
	{
		s_save_fat();
	}
	else
	{
		k_LogPtr += 12;
		if(k_LogPtr >= LOG_END_ADDR)
		{
			s_save_fat();
		}
	}
}


static void s_log_exdata(uint32_t sAddr)
{
	uint16_t buff[4];

	buff[0] = LOG_EXCHANGE_DATA;
	buff[1] = *((uint16_t *)&sAddr);
	buff[2] = *((uint16_t *)&sAddr+1);
	buff[3] = FILE_CMD_OK;

	if(__flash_write(k_LogPtr, buff, 4))
	{
		s_save_fat();
	}
	else
	{
		k_LogPtr += 4;
		if(k_LogPtr >= LOG_END_ADDR)
		{
			s_save_fat();
		}
	}
}

static uint16_t s_get_free_block(void)
{
	uint16_t      tmpBlock;

	tmpBlock = k_FreeBlockPtr;
	k_FreeBlockPtr = k_FAT[tmpBlock];
	k_FreeBlockNum--;

	return(tmpBlock);
}

static void s_chk_used_block(int32_t BlockNum)
{
	int32_t       i, j;
	uint16_t      upPtr = 0, tmpPtr;
	uint16_t      *datPtr;

	tmpPtr = k_FreeBlockPtr;
	for(j=0; j<BlockNum; j++)
	{
		if(tmpPtr == NULL_BLOCK)
		{
			break;
		}
		datPtr = (uint16_t *)(DATA_ADDR + tmpPtr*BLOCK_SIZE);

		for(i=0; i<BLOCK_SIZE/2; i++)
		{
            uint16_t temp_uint16;
            __flash_readw(&temp_uint16, datPtr);
            datPtr++;
			if(temp_uint16 != 0xffff)
			{
				break;
			}
		}
		if(i < BLOCK_SIZE/2)
		{
			k_UsedBlockNum++;
			k_FreeBlockNum--;
			if(tmpPtr == k_FreeBlockPtr)
			{
				k_FreeBlockPtr = k_FAT[tmpPtr];
				k_FAT[tmpPtr] = USED_BLOCK;
				tmpPtr = k_FreeBlockPtr;
			}
			else
			{
				k_FAT[upPtr] = k_FAT[tmpPtr];
				k_FAT[tmpPtr] = USED_BLOCK;
				tmpPtr = k_FAT[upPtr];
			}
		}
		else
		{
			upPtr = tmpPtr;
			tmpPtr = k_FAT[tmpPtr];
		}
	}
}

static void s_copy_secdata(uint32_t dAddr, uint32_t sAddr, uint16_t *blkPtr,
							   int32_t blkNo, int32_t len, uint8_t ch, int32_t oddFlag)
{
	int32_t       i, j, k;
	uint16_t      *sPtr, *dPtr, *tmpPtr, *bPtr;
	uint8_t       buff[2];

	for(k=0; k<3; k++)
	{
		sPtr = (uint16_t *)sAddr;
		dPtr = (uint16_t *)dAddr;
		bPtr = blkPtr;
		__flash_erase(dAddr);

		for(i=0; i<SECTOR_BLOCK; i++)
		{
			if(*bPtr++ != USED_BLOCK)
			{
				tmpPtr = sPtr;
				for(j=0; j<BLOCK_SIZE/2; j++)
				{
                    uint16_t temp_uint16;
                    __flash_readw(&temp_uint16, tmpPtr);
                    tmpPtr++;
					if(temp_uint16 != 0xffff)
					{
					    break;
					}
				}

				if(j < BLOCK_SIZE/2)
				{
					if(blkNo == i)
					{
						len /= 2;
						if(__flash_write(dPtr, sPtr, len))
						{
							break;
						}
						if(oddFlag)
						{
							buff[0] = ch;
							buff[1] = 0xff;
							if(__flash_write(dPtr+len, (uint16_t *)buff, 1))
							{
								break;
							}
						}
					}
					else if(__flash_write(dPtr, sPtr, BLOCK_SIZE/2))
					{
						break;
					}
				}
			}
			sPtr += BLOCK_SIZE/2;
			dPtr += BLOCK_SIZE/2;
		}

		if(i == SECTOR_BLOCK)
		{
			return;
		}
	}
	__flash_write_failed();
}

void s_deal_wr_error(uint16_t *des, uint16_t *src, int32_t len, uint8_t ch,
						int32_t oddFlag)
{
	int32_t       offset, blkNo, SecNo;
	uint32_t      tmp, sAddr;
	uint16_t      *blkPtr, buff[4];

	SecNo = ((uint32_t)des - DATA_ADDR)/DATA_SECTOR_SIZE;
	tmp = ((uint32_t)des - DATA_ADDR)%DATA_SECTOR_SIZE;
	blkNo = tmp/BLOCK_SIZE;
	offset = tmp%BLOCK_SIZE;
	sAddr = DATA_ADDR + SecNo*DATA_SECTOR_SIZE;
	blkPtr = k_FAT + SecNo*SECTOR_BLOCK;
	s_copy_secdata(k_ExchangeAddr, sAddr, blkPtr, blkNo, offset, ch, oddFlag);

	buff[0] = LOG_FWRITE_FAILED;
	memcpy(buff+1, &sAddr, 4);
	buff[3] = FILE_CMD_OK;
	__flash_write(k_FailLogPtr, buff, 4);
	k_FailLogPtr += 4;
	if(k_FailLogPtr > (uint16_t *)(LOG_ADDR+LOG_SECTOR_SIZE-16))
	{
		__flash_write_failed();
	}

	s_copy_secdata(sAddr,k_ExchangeAddr, blkPtr, blkNo, offset, ch, oddFlag);

	if(__flash_write(des, src, len))
	{
		__flash_write_failed();
    }
}

static void s_wr_block(uint16_t BlockPtr, int32_t offset, uint8_t *dat, int32_t len)
{
	uint8_t       tmpBuf[BLOCK_SIZE];
	uint32_t      Addr;

	Addr = DATA_ADDR + BlockPtr*BLOCK_SIZE + offset;
	if(offset % 2)
	{
		Addr--;
		if((uint32_t)dat % 2)
		{
			tmpBuf[0] = *(uint8_t *)(Addr);
			tmpBuf[1] = *dat++;
			len--;

			if(__flash_write((uint16_t *)(Addr), (uint16_t *)tmpBuf, 1))
			{
				s_deal_wr_error((uint16_t *)(Addr), (uint16_t *)tmpBuf, 1, tmpBuf[0], 1);
			}

			if(__flash_write((uint16_t *)(Addr+2), (uint16_t *)dat, len/2))
			{
				s_deal_wr_error((uint16_t *)(Addr+2), (uint16_t *)dat, len/2, 0, 0);
			}

			if(len % 2)
			{
				tmpBuf[0] = dat[len-1];
				tmpBuf[1] = 0xff;

				if(__flash_write((uint16_t *)(Addr+len+1), (uint16_t *)tmpBuf, 1))
				{
					s_deal_wr_error((uint16_t *)(Addr+len+1), (uint16_t *)tmpBuf, 1, 0, 0);
				}
			}
			return;
		}

		__flash_readb(&tmpBuf[0], Addr);
		memcpy(tmpBuf+1, dat, len);

		if(len%2 == 0)
		{
			tmpBuf[len+1] = 0xff;
		}
		if(__flash_write((uint16_t *)(Addr), (uint16_t *)tmpBuf, len/2+1))
		{
			s_deal_wr_error((uint16_t *)(Addr), (uint16_t *)tmpBuf, len/2+1, tmpBuf[0], 1);
		}
		return;
	}

	if((uint32_t)dat % 2)
	{
		memcpy(tmpBuf, dat, len);
		if(len % 2)
		{
			tmpBuf[len] = 0xff;
			len = len/2+1;
		}
		else
		{
			len = len/2;
		}
		if(__flash_write((uint16_t *)(Addr), (uint16_t *)tmpBuf, len))
		{
			s_deal_wr_error((uint16_t *)(Addr), (uint16_t *)tmpBuf, len, 0, 0);
		}

		return;
	}

	if(__flash_write((uint16_t *)(Addr), (uint16_t *)dat, len/2))
	{
		s_deal_wr_error((uint16_t *)(Addr), (uint16_t *)dat, len/2, 0, 0);
	}

	if(len % 2)
	{
		tmpBuf[0] = dat[len-1];
		tmpBuf[1] = 0xff;
		if(__flash_write((uint16_t *)(Addr+len-1), (uint16_t *)tmpBuf, 1))
		{
			s_deal_wr_error((uint16_t *)(Addr + len-1), (uint16_t *)tmpBuf, 1, 0, 0);
		}
	}
}

static int s_wr_blockEx(uint16_t BlockPtr, int32_t offset, uint8_t *dat, int32_t len)
{
    uint8_t *k_filetptr = NULL;
    k_filetptr = malloc(BLOCK_SIZE);
    if ( k_filetptr == NULL ) {
        return 1;
    }
    __flash_read(k_filetptr, dat, len);
    s_wr_block(BlockPtr, offset, k_filetptr, len);
    free(k_filetptr);
    return 0;
}

static int32_t  s_search_file(char *shortname, int belong_app)
{
	int32_t   i, len;

    len = strlen(shortname);
	for(i=0; i<MAX_FILES; i++)
	{
		if(belong_app == k_Flist[i].belong_app)
		{
			if(len < 16)
			{
				if(!strcmp((char *)shortname, (char *)k_Flist[i].file_name))
				{
					return(i);
				}
			}
			else if(!memcmp(shortname, k_Flist[i].file_name, 16))
			{
				return(i);
			}
		}
	}

	return(-FS_NOTEXIST);
}

static void s_creat_file(uint16_t fd, int8_t *shortname, int32_t belong_app)
{
	FILE_ITEM   *fItemPtr;

	fItemPtr = &k_Flist[fd];
	memcpy(fItemPtr->file_name, shortname, 16);
	fItemPtr->belong_app = belong_app;
	fItemPtr->StartBlock = NULL_BLOCK;
	fItemPtr->size = 0;
	if(!k_fsRecover)
	{
		s_log_creat(fd, shortname, belong_app);
	}
}


static void s_remove_by_id(int fd)
{
	uint16_t      tmpPtr, BlockPtr;

	k_Flist[fd].file_name[0] = 0;
	k_Flist[fd].belong_app = FILE_BELONG_DELETE;
	k_Fstatus[fd].OpenFlag = 0;
	BlockPtr = k_Flist[fd].StartBlock;

	while(BlockPtr != NULL_BLOCK)
	{
		tmpPtr = BlockPtr;
		BlockPtr = k_FAT[BlockPtr];
		k_FAT[tmpPtr] = USED_BLOCK;
		k_UsedBlockNum++;
	}

	if(!k_fsRecover)
	{
		s_log_remove((uint16_t)fd);
	}
}

static void s_rename_by_id(int fd, char *new_shortname, int belong_app)
{
    memset(k_Flist[fd].file_name, 0x00, sizeof(k_Flist[fd].file_name));
    strncpy((char *)k_Flist[fd].file_name, new_shortname, 16);
	k_Flist[fd].belong_app = belong_app;

	if(!k_fsRecover)
	{
		s_log_rename((uint16_t)fd, (uint8_t *)new_shortname, belong_app);
	}
}

static int s_chk_fileid(int fd)
{
	if((fd < 0) || (fd >= MAX_FILES))
	{
        return(-FS_INVAL);
	}

	if(!k_Fstatus[fd].OpenFlag)
	{
        return(-FS_NOTOPENED);
	}
	return(OK);
}

static void  s_seek_file(int fd, int offset, int origin)
{
	FILE_ITEM   *fItemPtr;
	FILE_STATUS *fStatusPtr;
	uint16_t      tmpBlock, upBlock;

	fItemPtr = &k_Flist[fd];
	fStatusPtr = &k_Fstatus[fd];
	if(origin == SEEK_SET)
	{
		tmpBlock = fItemPtr->StartBlock;
		fStatusPtr->FilePtr = offset;
	}
	else if(origin == SEEK_END)
	{
		tmpBlock = fItemPtr->StartBlock;
		offset += fItemPtr->size;
		fStatusPtr->FilePtr=offset;
	}
	else
	{
		if(offset <= 0)
		{
			if((fStatusPtr->FilePtrOff + offset) >= 0)
			{
				fStatusPtr->FilePtr += offset;
				fStatusPtr->FilePtrOff += (int16_t)offset;
				return;
			}
			tmpBlock = fItemPtr->StartBlock;
			offset += fStatusPtr->FilePtr;
			fStatusPtr->FilePtr = offset;
		}
		else
		{
			if((fStatusPtr->FilePtrOff + offset) <= BLOCK_SIZE)
			{
				fStatusPtr->FilePtrOff += (int16_t)offset;
				fStatusPtr->FilePtr += offset;
				return;
			}
			fStatusPtr->FilePtr += offset;
			offset += fStatusPtr->FilePtrOff;
			tmpBlock = fStatusPtr->CurBlockPtr;
		}
	}
	upBlock = NULL_BLOCK;
	while(offset > BLOCK_SIZE)
	{
		upBlock = tmpBlock;
		tmpBlock = k_FAT[tmpBlock];
		offset -= BLOCK_SIZE;
	}

	fStatusPtr->UpBlockPtr = upBlock;
	fStatusPtr->CurBlockPtr = tmpBlock;

	if(tmpBlock!=NULL_BLOCK)
	{
		fStatusPtr->FilePtrOff = (int16_t)offset;
	}
}

static void s_ex_page(uint32_t sAddr)
{
	int32_t       i, j, k, sSecNo, dBlkTop, sBlkTop, sBlkBot;
	uint16_t      *src, *dec, *BlkPtr;
	FILE_ITEM   *fItemPtr;
	FILE_STATUS *fStatusPtr;

	sSecNo = (sAddr-DATA_ADDR) / DATA_SECTOR_SIZE;
	dBlkTop = ((k_ExchangeAddr-DATA_ADDR) / DATA_SECTOR_SIZE) * SECTOR_BLOCK;
	sBlkTop = sSecNo * SECTOR_BLOCK;
	sBlkBot = sBlkTop + SECTOR_BLOCK;
	memcpy(k_FAT+dBlkTop, k_FAT+sBlkTop, SECTOR_BLOCK*2);

	k = dBlkTop - sBlkTop;
	BlkPtr = k_FAT;
	for(i=0; i<DATA_SECTORS; i++)
	{
		if(i == sSecNo)
		{
			BlkPtr += SECTOR_BLOCK;
			continue;
		}

		for(j=0; j<SECTOR_BLOCK; j++)
		{
			if((*BlkPtr >= sBlkTop) && (*BlkPtr < sBlkBot))
			{
				*BlkPtr += k;
			}
			BlkPtr++;
		}
	}

	for(i=0; i<MAX_FILES; i++)
	{
		if(k_Flist[i].file_name[0])
		{
			fItemPtr = &k_Flist[i];
			if((fItemPtr->StartBlock >= sBlkTop) && (fItemPtr->StartBlock < sBlkBot))
			{
				fItemPtr->StartBlock += k;
			}

			if(k_Fstatus[i].OpenFlag)
			{
				fStatusPtr = &k_Fstatus[i];

				if((fStatusPtr->UpBlockPtr >= sBlkTop) && (fStatusPtr->UpBlockPtr < sBlkBot))
				{
					fStatusPtr->UpBlockPtr += k;
				}
				if((fStatusPtr->CurBlockPtr >= sBlkTop) && (fStatusPtr->CurBlockPtr < sBlkBot))
				{
                    fStatusPtr->CurBlockPtr += k;
				}
			}
		}
	}

	if((k_FreeBlockPtr >= sBlkTop) && (k_FreeBlockPtr < sBlkBot))
	{
		k_FreeBlockPtr += k;
	}

	for(j=0; j<3; j++)

	{
		if(!k_fsRecover)
		{
			__flash_erase(k_ExchangeAddr);
		}

		BlkPtr = k_FAT + dBlkTop;
		src = (uint16_t *)sAddr;
		dec = (uint16_t *)k_ExchangeAddr;

		for(i=0; i<SECTOR_BLOCK; i++)
		{
			if(*BlkPtr == USED_BLOCK)
			{
				k_UsedBlockNum--;
				k_FreeBlockNum++;
				*BlkPtr = k_FreeBlockPtr;
				k_FreeBlockPtr = dBlkTop+i;
			}
			else if(!k_fsRecover)
			{
#ifdef SRC_DEC_SPACE_CONFLIC
                uint16_t buff[BLOCK_SIZE / 2];
                __flash_read(buff, src, BLOCK_SIZE);
				if(__flash_write(dec, buff, BLOCK_SIZE/2))
				{
					break;
				}
#else
				if(__flash_write(dec, src, BLOCK_SIZE/2))
				{
					break;
				}
#endif
			}

			BlkPtr++;
			dec += BLOCK_SIZE/2;
			src += BLOCK_SIZE/2;
		}

		if(i == SECTOR_BLOCK)
		{
			break;
		}
	}

	if(j == 3)
	{
		__flash_write_failed();
	}
	k_ExchangeAddr = sAddr;
	if(!k_fsRecover)
	{
		s_log_exdata(sAddr);
	}
}

static void s_recycle_block(void)
{
	int32_t   i, j, k, s, n, end, BlkNo = 0;
	uint16_t  *BlkPtr, UsedBlk;

	UsedBlk = 0;
	j = (k_ExchangeAddr-DATA_ADDR) / DATA_SECTOR_SIZE;

	for(n=0; n<2; n++)
	{
		if(n)
		{
			i = 0;
			end = j;
			BlkPtr = k_FAT;
		}
		else
		{
			i = j + 1;
			end = DATA_SECTORS;
			BlkPtr = k_FAT + i*SECTOR_BLOCK;
		}

		for(; i<end; i++)
		{
			s = 0;
			for(k = 0; k < SECTOR_BLOCK; k++)
			{
				if(*BlkPtr++ == USED_BLOCK)
				{
					s++;
				}
			}

			if(s > UsedBlk)
			{
				UsedBlk = s;
				BlkNo = i;
			}

			if(UsedBlk == SECTOR_BLOCK)
			{
				break;
			}
		}

		if(UsedBlk == SECTOR_BLOCK)
		{
			break;
		}
	}

	if(!UsedBlk)
	{
		return;
	}
	s_ex_page(DATA_ADDR + BlkNo*DATA_SECTOR_SIZE);
}

static int32_t s_write_file(int32_t fd, uint8_t *dat)
{
	int32_t           len,tmpLen,needBlocks;
	FILE_ITEM       *fItemPtr;
	FILE_STATUS     *fStatusPtr;
	uint16_t          oldPtr,newPtr,upPtr,tmpPtr;
	uint8_t           *charPtr;
        uint16_t temp_uint16;

    if(dat == NULL)
    {
		return(-FS_INVAL);
    }
	fItemPtr   = &k_Flist[fd];
	fStatusPtr = &k_Fstatus[fd];
	len        = fStatusPtr->len;
	tmpLen     = fStatusPtr->FilePtr%BLOCK_SIZE;    //  当前指针在block位置，0等效于blocksize
	if(tmpLen)      //  不在块起始位置
	{
		tmpLen = len - (BLOCK_SIZE-tmpLen);
		if(tmpLen <= 0)
		{
			needBlocks = 1;     //  本块空间够写
		}
		else if(tmpLen % BLOCK_SIZE)
		{
			needBlocks = tmpLen/BLOCK_SIZE + 2;
		}
		else
		{
			needBlocks = tmpLen/BLOCK_SIZE + 1;
		}
	}
	else
	{
		if(len % BLOCK_SIZE)
		{
			needBlocks = len/BLOCK_SIZE + 2;
		}
		else
		{
			needBlocks = len/BLOCK_SIZE + 1;
		}
	}

	if(needBlocks > k_FreeBlockNum)
	{
		if(needBlocks > (k_FreeBlockNum+k_UsedBlockNum))
		{
            return(-FS_NOSPACE);
		}

		while(needBlocks > k_FreeBlockNum)
		{
            s_recycle_block();
		}
	}

	if(!k_fsRecover)
	{
		if(__flash_write(k_LogPtr, (uint16_t *)fStatusPtr, 10))
		{
			s_save_fat();
			if(__flash_write(k_LogPtr, (uint16_t *)fStatusPtr, 10))
			{
				__flash_write_failed();
			}
		}
		k_LogPtr    += 9;
		k_FailLogPtr = k_LogPtr+1;
	}
	if(fStatusPtr->FilePtr == fItemPtr->size)   //  文件指针在文件的末尾，追加写
	{
		if(fItemPtr->size == 0)     //  该文件还没有分配block
		{
			newPtr        = s_get_free_block();
			k_FAT[newPtr] = NULL_BLOCK;
			fItemPtr->StartBlock    = newPtr;
			fStatusPtr->FilePtrOff  = 0;
			fStatusPtr->UpBlockPtr  = NULL_BLOCK;
			fStatusPtr->CurBlockPtr = newPtr;
		}
		fStatusPtr->FilePtr += len;
		fItemPtr->size      += len;

		tmpLen = BLOCK_SIZE - fStatusPtr->FilePtrOff;
		if(tmpLen >= len)   //  本块够写
		{
			if(!k_fsRecover)
			{
                uint16_t temp_uint16;
				s_wr_block(fStatusPtr->CurBlockPtr, fStatusPtr->FilePtrOff, dat, len);
				s_log_ok();
                __flash_readw(&temp_uint16, k_LogPtr);
				if(temp_uint16 == LOG_FWRITE_FAILED)
				{
					k_LogPtr += 4;
				}
			}

			fStatusPtr->FilePtrOff += len;
			return 0;
		}
		else        //  本块不够写
		{
			if(tmpLen)      //  本块还有空间写
			{
				if(!k_fsRecover)
				{
					s_wr_block(fStatusPtr->CurBlockPtr, fStatusPtr->FilePtrOff, dat, tmpLen);
				}
				dat += tmpLen;
				len -= tmpLen;
			}
			upPtr = fStatusPtr->CurBlockPtr;
			while(1)        //  分配新块写
			{
				newPtr       = s_get_free_block();
				k_FAT[upPtr] = newPtr;
				if(len <= BLOCK_SIZE)
				{
					k_FAT[newPtr] = NULL_BLOCK;
					if(!k_fsRecover)
					{
                        
						s_wr_block(newPtr, 0, dat, len);
						s_log_ok();
						while(__flash_readw(&temp_uint16, k_LogPtr), temp_uint16 == LOG_FWRITE_FAILED)
						{
                            k_LogPtr += 4;
						}
					}
					fStatusPtr->CurBlockPtr = newPtr;
					fStatusPtr->UpBlockPtr  = upPtr;
					fStatusPtr->FilePtrOff  = len;
					return 0;
				}
				if(!k_fsRecover)
				{
                    s_wr_block(newPtr, 0, dat, BLOCK_SIZE);
				}
				dat  += BLOCK_SIZE;
				len  -= BLOCK_SIZE;
				upPtr = newPtr;
			}
		}
	}

	if(fStatusPtr->FilePtrOff == BLOCK_SIZE)        //  若定位到该位置则移到下一个block
	{
		fStatusPtr->UpBlockPtr  = fStatusPtr->CurBlockPtr;
		fStatusPtr->CurBlockPtr = k_FAT[fStatusPtr->CurBlockPtr];
		fStatusPtr->FilePtrOff  = 0;
	}

	if(fStatusPtr->FilePtr+len >= fItemPtr->size)   //  改写+追加
	{
		fItemPtr->size      = fStatusPtr->FilePtr+len;
		fStatusPtr->FilePtr = fItemPtr->size;

		newPtr = s_get_free_block();
		if(fStatusPtr->UpBlockPtr == NULL_BLOCK)
		{
            fItemPtr->StartBlock = newPtr;
		}
		else
		{
            k_FAT[fStatusPtr->UpBlockPtr] = newPtr;        //  让上一块指向新块，用来替换旧的
		}
		tmpLen = BLOCK_SIZE - fStatusPtr->FilePtrOff;
		if(tmpLen >= len)   //  本块够写，而且由上一个改写+追加条件判断本块一定是最后一块
		{
			k_FAT[fStatusPtr->CurBlockPtr] = USED_BLOCK;
			k_UsedBlockNum++;
			k_FAT[newPtr] = NULL_BLOCK;
			if(!k_fsRecover)
			{
                uint16_t temp_uint16;
				if(fStatusPtr->FilePtrOff)      // 如果是恢复日志,则本块内原先内容也会丢失
				{
//					s_wr_block(newPtr,0,(unsigned char *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
					s_wr_blockEx(newPtr,0,(unsigned char *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
				}
				s_wr_block(newPtr, fStatusPtr->FilePtrOff, dat, len);
				s_log_ok();
                __flash_readw(&temp_uint16, k_LogPtr);
				if(temp_uint16 == LOG_FWRITE_FAILED)
				{
                    k_LogPtr += 4;
				}
			}
			fStatusPtr->FilePtrOff += len;
			fStatusPtr->CurBlockPtr = newPtr;
			return 0;
		}
		if(!k_fsRecover)
		{
			if(fStatusPtr->FilePtrOff)
			{
//				s_wr_block(newPtr,0,(unsigned char *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
				s_wr_blockEx(newPtr,0,(unsigned char *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
			}
            s_wr_block(newPtr, fStatusPtr->FilePtrOff, dat, tmpLen);
		}
		dat += tmpLen;
		len -= tmpLen;

		while(1)
		{
			upPtr  = newPtr;
			newPtr = s_get_free_block();
			k_FAT[upPtr] = newPtr;
			if(len <= BLOCK_SIZE)
			{
				break;
			}
			if(!k_fsRecover)
			{
				s_wr_block(newPtr, 0, dat, BLOCK_SIZE);
			}
			dat += BLOCK_SIZE;
			len -= BLOCK_SIZE;
		}

		k_FAT[newPtr] = NULL_BLOCK;
		fStatusPtr->UpBlockPtr  = upPtr;
		fStatusPtr->FilePtrOff  = len;
		tmpPtr = fStatusPtr->CurBlockPtr;
		fStatusPtr->CurBlockPtr = newPtr;
		while(tmpPtr != NULL_BLOCK)       //  由于剩余空间不够写，因此旧块均可以回收
		{
			upPtr        = tmpPtr;
			tmpPtr       = k_FAT[tmpPtr];
			k_FAT[upPtr] = USED_BLOCK;
			k_UsedBlockNum++;
		}
		if(!k_fsRecover)
		{
            uint16_t temp_uint16;
			s_wr_block(newPtr, 0, dat, len);
			s_log_ok();
			while(__flash_readw(&temp_uint16, k_LogPtr), temp_uint16 == LOG_FWRITE_FAILED)
			{
				k_LogPtr += 4;
			}
		}
		return 0;
	}
	//  仅仅是改写数据
	fStatusPtr->FilePtr += len;
	newPtr = s_get_free_block();
	if(fStatusPtr->UpBlockPtr == NULL_BLOCK)
	{
		fItemPtr->StartBlock = newPtr;
	}
	else
	{
		k_FAT[fStatusPtr->UpBlockPtr] = newPtr;
	}
	tmpPtr = k_FAT[fStatusPtr->CurBlockPtr];
	k_FAT[fStatusPtr->CurBlockPtr] = USED_BLOCK;
	k_UsedBlockNum++;
	tmpLen = BLOCK_SIZE - fStatusPtr->FilePtrOff;
	if(tmpLen >= len)   //  本块够写
	{
		k_FAT[newPtr] = tmpPtr;
		if(!k_fsRecover)
		{
            uint16_t temp_uint16;
			charPtr = (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr * BLOCK_SIZE);
			if(fStatusPtr->FilePtrOff)
			{
//				s_wr_block(newPtr, 0, charPtr, fStatusPtr->FilePtrOff);
				s_wr_blockEx(newPtr, 0, charPtr, fStatusPtr->FilePtrOff);
			}
			s_wr_block(newPtr, fStatusPtr->FilePtrOff, dat, len);
			tmpLen -= len;
			len    += fStatusPtr->FilePtrOff;
			if(tmpLen)
			{
//				s_wr_block(newPtr, len, charPtr+len, tmpLen);
				s_wr_blockEx(newPtr, len, charPtr+len, tmpLen);
			}
			s_log_ok();

            __flash_readw(&temp_uint16, k_LogPtr);
			if(temp_uint16 == LOG_FWRITE_FAILED)
			{
				k_LogPtr += 4;
			}
		}

		fStatusPtr->CurBlockPtr = newPtr;
		fStatusPtr->FilePtrOff  = len;
		return 0;
	}

	if(!k_fsRecover)
	{
		if(fStatusPtr->FilePtrOff)
		{
//			s_wr_block(newPtr, 0, (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr * BLOCK_SIZE), fStatusPtr->FilePtrOff);
			s_wr_blockEx(newPtr, 0, (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr * BLOCK_SIZE), fStatusPtr->FilePtrOff);
		}
		s_wr_block(newPtr, fStatusPtr->FilePtrOff, dat, tmpLen);
	}

	len -= tmpLen;
	dat += tmpLen;

	upPtr = newPtr;
	while(len >= BLOCK_SIZE)
	{
		newPtr       = s_get_free_block();
		k_FAT[upPtr] = newPtr;
		if(!k_fsRecover)
		{
            s_wr_block(newPtr, 0, dat, BLOCK_SIZE);
		}
		dat   += BLOCK_SIZE;
		len   -= BLOCK_SIZE;
		upPtr  = newPtr;
		oldPtr = tmpPtr;
		tmpPtr = k_FAT[tmpPtr];
		k_FAT[oldPtr] = USED_BLOCK;
		k_UsedBlockNum++;
	}
	if(len)
	{
		newPtr       = s_get_free_block();
		k_FAT[upPtr] = newPtr;
		if(!k_fsRecover)
		{
			s_wr_block(newPtr, 0, dat, len);
//			s_wr_block(newPtr, len, (uint8_t *)(DATA_ADDR+tmpPtr*BLOCK_SIZE+len), BLOCK_SIZE-len);
			s_wr_blockEx(newPtr, len, (uint8_t *)(DATA_ADDR+tmpPtr*BLOCK_SIZE+len), BLOCK_SIZE-len);
		}

		k_FAT[newPtr] = k_FAT[tmpPtr];
		k_FAT[tmpPtr] = USED_BLOCK;
		k_UsedBlockNum++;
		fStatusPtr->CurBlockPtr = newPtr;
		fStatusPtr->UpBlockPtr  = upPtr;
		fStatusPtr->FilePtrOff  = len;
	}
	else
	{
		k_FAT[newPtr]           = tmpPtr;
		fStatusPtr->CurBlockPtr = tmpPtr;
		fStatusPtr->UpBlockPtr  = newPtr;
		fStatusPtr->FilePtrOff  = 0;
	}
	if(!k_fsRecover)
	{
        uint16_t temp_uint16;
		s_log_ok();
		while(__flash_readw(&temp_uint16, k_LogPtr), temp_uint16 == LOG_FWRITE_FAILED)
		{
			k_LogPtr += 4;
		}
	}
	return 0;
}

static void  s_truncate_file(int32_t fd, int32_t len)
{
	uint16_t      headPtr,tmpPtr,newPtr;
	FILE_ITEM   *fItemPtr;
	FILE_STATUS *fStatusPtr;

	fItemPtr = &k_Flist[fd];
	fStatusPtr = &k_Fstatus[fd];

	if(len%BLOCK_SIZE)
	{
		if(!k_FreeBlockNum)
		{
			s_recycle_block();
		}
		newPtr = s_get_free_block();
		k_FAT[newPtr] = NULL_BLOCK;
		s_seek_file(fd,len,SEEK_SET);
		headPtr = fStatusPtr->CurBlockPtr;
		if(fStatusPtr->UpBlockPtr == NULL_BLOCK)
		{
			fItemPtr->StartBlock = newPtr;
		}
		else
		{
			k_FAT[fStatusPtr->UpBlockPtr] = newPtr;
		}
		if(!k_fsRecover)
		{
			s_log_truncate((uint16_t)fd,len);
			k_FailLogPtr = k_LogPtr+1;
//			s_wr_block(newPtr,0,(uint8_t *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
			s_wr_blockEx(newPtr,0,(uint8_t *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE),fStatusPtr->FilePtrOff);
		}
		fStatusPtr->CurBlockPtr=newPtr;
	}
	else
	{
		if(!k_fsRecover)
		{
			s_log_truncate((uint16_t)fd, len);
		}

		if(len == 0)
		{
			headPtr = fItemPtr->StartBlock;
			fStatusPtr->FilePtrOff = BLOCK_SIZE;
			fItemPtr->StartBlock = NULL_BLOCK;
			fStatusPtr->CurBlockPtr = NULL_BLOCK;
			fStatusPtr->UpBlockPtr = NULL_BLOCK;
		}
		else
		{
			s_seek_file(fd, len, SEEK_SET);
			headPtr = k_FAT[fStatusPtr->CurBlockPtr];
			k_FAT[fStatusPtr->CurBlockPtr] = NULL_BLOCK;
		}
	}
	while(headPtr != NULL_BLOCK)
	{
		tmpPtr = headPtr;
		headPtr = k_FAT[headPtr];
		k_FAT[tmpPtr] = USED_BLOCK;
		k_UsedBlockNum++;
	}
	fItemPtr->size = len;
	fStatusPtr->FilePtr = len;
	if(!k_fsRecover)
	{
        uint16_t temp_uint16;
		s_log_ok();
        __flash_readw(&temp_uint16, k_LogPtr);
		if(temp_uint16 == LOG_FWRITE_FAILED)
		{
			k_LogPtr += 4;
		}
	}
}

int fs_open(const char *file_name, int open_mode)
{
	int         fd;
	FILE_STATUS *fStatusPtr;
    char        shortname[17];
    int         belong_app;
    int         ret;

    if (open_mode & (~(O_RDWR | O_CREAT)) || (!(open_mode & (O_RDWR | O_CREAT))))
    {
		return(-FS_INVAL);
    }

    if (file_name == NULL)
    {
		return(-FS_INVAL);
    }

    memset(shortname, 0x00, sizeof(shortname));
    ret = s_fname_convert(file_name, shortname, &belong_app);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }

	fd = s_search_file(shortname, belong_app);
	if(fd >= 0)
	{
		if(open_mode == O_CREAT)
		{
            return(-FS_EXIST);
		}
	}
	else
	{
		if((open_mode & O_CREAT) != O_CREAT)
		{
			return(-FS_NOTEXIST);
		}

		for(fd=0; fd<MAX_FILES; fd++)
		{
			if(!k_Flist[fd].file_name[0])
			{
				break;
			}
		}

		if(fd == MAX_FILES)
		{
            return(-FS_MAXFILENO);
		}

		s_creat_file((uint16_t)fd, (int8_t *)shortname, belong_app);
	}

	fStatusPtr = &k_Fstatus[fd];
	fStatusPtr->fd = fd;
	fStatusPtr->CmdSet = FILE_CMD_SET;
	fStatusPtr->OpenFlag = LOG_WRITE_FILE;
	fStatusPtr->CurBlockPtr = k_Flist[fd].StartBlock;
	fStatusPtr->UpBlockPtr = NULL_BLOCK;
	fStatusPtr->FilePtr = 0;
	if(fStatusPtr->CurBlockPtr == NULL_BLOCK)
	{
		fStatusPtr->FilePtrOff = BLOCK_SIZE;
	}
	else
	{
		fStatusPtr->FilePtrOff = 0;
	}

	return(fd);
}

int fs_close(int fileno)
{
    int32_t     ret;

    ret = s_chk_fileid(fileno);
	if(ret < 0)
	{
		return(ret);
	}
	k_Fstatus[fileno].OpenFlag = 0;

	return(0);
}

int fs_seek(int fileno, int offset, int whence)
{
	int32_t       tmpLen;
    int32_t     ret;

    ret = s_chk_fileid(fileno);
	if(ret < 0)
	{
		return(ret);
	}

	if(whence == SEEK_SET)
	{
		tmpLen = 0;
	}
	else if(whence == SEEK_CUR)
	{
		tmpLen = k_Fstatus[fileno].FilePtr;
	}
	else if(whence == SEEK_END)
	{
		tmpLen = k_Flist[fileno].size;
	}
	else
	{
		return(-FS_INVAL);
	}

	if((tmpLen + offset) < 0)
	{
		offset = 0 - tmpLen;
	}

	if((tmpLen + offset) > k_Flist[fileno].size)
	{
		offset = k_Flist[fileno].size - tmpLen;
	}

	s_seek_file(fileno, offset, whence);
    return(k_Fstatus[fileno].FilePtr);
}

int fs_read(int fileno, void *outbuf, int bytelen)
{
	int32_t       bakLen;
	uint16_t      tmpLen,tmpBlock,upBlock;
	FILE_ITEM   *fItemPtr;
	FILE_STATUS *fStatusPtr;
    int         ret;
    uint8_t     *poutbuf = outbuf;

    if (outbuf == NULL || bytelen < 0)
    {
		return(-FS_INVAL);
    }

    ret = s_chk_fileid(fileno);
	if(ret < 0)
	{
		return(ret);
	}

	fItemPtr = &k_Flist[fileno];
	fStatusPtr = &k_Fstatus[fileno];

	if(fStatusPtr->FilePtr+bytelen > fItemPtr->size)
	{
		bytelen = fItemPtr->size - fStatusPtr->FilePtr;
	}

	if(!bytelen)
	{
		return(0);
	}
    if(poutbuf == NULL)
    {
		return(-FS_INVAL);
    }

	tmpLen = BLOCK_SIZE-fStatusPtr->FilePtrOff;

	if(bytelen <= tmpLen)
	{
		__flash_read(poutbuf, (uint8_t *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE+fStatusPtr->FilePtrOff), bytelen);
		fStatusPtr->FilePtrOff += bytelen;
		fStatusPtr->FilePtr += bytelen;
		return(bytelen);
	}

	bakLen = bytelen;
	if(tmpLen)
	{
		__flash_read(poutbuf, (uint8_t *)(DATA_ADDR+fStatusPtr->CurBlockPtr*BLOCK_SIZE+fStatusPtr->FilePtrOff), tmpLen);
		bytelen -= tmpLen;
		poutbuf += tmpLen;
	}

	tmpBlock = fStatusPtr->CurBlockPtr;
	while(1)
	{
		upBlock = tmpBlock;
		tmpBlock = k_FAT[tmpBlock];
		if(bytelen <= BLOCK_SIZE)
		{
			__flash_read(poutbuf, (uint8_t *)(DATA_ADDR + tmpBlock*BLOCK_SIZE), bytelen);
			break;
		}
		__flash_read(poutbuf, (uint8_t *)(DATA_ADDR+tmpBlock*BLOCK_SIZE), BLOCK_SIZE);
		bytelen -= BLOCK_SIZE;
		poutbuf += BLOCK_SIZE;
	}

	fStatusPtr->FilePtr += bakLen;
	fStatusPtr->FilePtrOff = bytelen;
	fStatusPtr->CurBlockPtr = tmpBlock;
	fStatusPtr->UpBlockPtr = upBlock;
	return(bakLen);
}

int fs_write(int fileno, const void *inbuf, int bytelen)
{
    int32_t     ret;

    if (inbuf == NULL)
    {
		return(-FS_INVAL);
    }

    ret = s_chk_fileid(fileno);
	if(ret < 0)
	{
		return(ret);
	}
	if(bytelen < 0)
	{
		return(-FS_INVAL);
	}
	if(bytelen == 0)
	{
		return 0;
	}


    k_Fstatus[fileno].len = bytelen;
	ret = s_write_file(fileno, (uint8_t *)inbuf);
	if(ret < 0)
	{
		return ret;
	}
	else
	{
	    return(bytelen);
	}
}

int fs_size(const char *file_name)
{
	int         fd;
    char        shortname[17];
    int         belong_app;
    int         ret;

    if (file_name == NULL
            || strcmp(file_name, "") == 0)
    {
		return(-FS_INVAL);
    }

    memset(shortname, 0x00, sizeof(shortname));
    ret = s_fname_convert(file_name, shortname, &belong_app);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }

	fd = s_search_file(shortname, belong_app);
	if(fd < 0)
	{
		return(fd);
	}

	return(k_Flist[fd].size);
}

int fs_exist(const char *file_name)
{
	int         fd;
    char        shortname[17];
    int         belong_app;
    int         ret;

    if (file_name == NULL
            || strcmp(file_name, "") == 0)
    {
		return(-FS_INVAL);
    }

    memset(shortname, 0x00, sizeof(shortname));
    ret = s_fname_convert(file_name, shortname, &belong_app);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }

	fd = s_search_file(shortname, belong_app);
	return(fd);
}

int fs_truncate(int fileno)
{
    int     ret;
    int     remainlen;

    ret = s_chk_fileid(fileno);
	if(ret < 0)
	{
		return(ret);
	}

    remainlen = fs_seek(fileno, 0, SEEK_CUR);

    if(remainlen < k_Flist[fileno].size)
	{
    	s_truncate_file(fileno, remainlen);
	}

	return(k_Flist[fileno].size);
}

int fs_remove(const char *file_name)
{
	int         fd;
    char        shortname[17];
    int         belong_app;
    int         ret;

    if (file_name == NULL
            || strcmp(file_name, "") == 0)
    {
		return(-FS_INVAL);
    }

    memset(shortname, 0x00, sizeof(shortname));
    ret = s_fname_convert(file_name, shortname, &belong_app);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }

	fd = s_search_file(shortname, belong_app);
	if(fd < 0)
	{
        return(fd);
	}

	if(k_Fstatus[fd].OpenFlag)
	{
		return(-FS_OPENED);
	}

	s_remove_by_id(fd);
	return(0);
}

int fs_rename(const char *oldname, const char *newname)
{
    // 不支持文件重命令
    return(-FS_INVAL);
#if 0
	int     fd;
    int     ret;
    int     oldbelong, newbelong;
    char    old_shortname[17], new_shortname[17];
    if (oldname == NULL)
    {
		return(-FS_INVAL);
    }

    if (newname == NULL)
    {
		return(-FS_INVAL);
    }

    memset(old_shortname, 0x00, sizeof(old_shortname));
    memset(new_shortname, 0x00, sizeof(new_shortname));
    ret = s_fname_convert(oldname, old_shortname, &oldbelong);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }
    ret = s_fname_convert(newname, new_shortname, &newbelong);
    if(ret < 0)
    {
        return(-FS_INVAL);
    }

	fd = s_search_file(old_shortname, oldbelong);
	if(fd < 0)
	{
        return(-FS_NOTEXIST);
	}
	if(k_Fstatus[fd].OpenFlag)
	{
		return(-FS_OPENED);
	}

    if((strcmp(old_shortname, new_shortname) == 0) && (oldbelong == newbelong))
    {
        return(0);
    }

    if(s_search_file(new_shortname, newbelong) >= 0)
    {
        return(-FS_EXIST);
    }

	s_rename_by_id(fd, new_shortname, newbelong);

	return(0);
#endif
}

int fs_freespace(void)
{
	return((k_FreeBlockNum+k_UsedBlockNum-1) * BLOCK_SIZE);
}

int fs_list(int begno, file_attr_t *fileattr)
{
    int     valid_num = 0;
    int     i;

    if (fileattr == NULL || begno < 0)
    {
		return(-FS_INVAL);
    }

    for(i=0; i<MAX_FILES; i++)
    {
        if(k_Flist[i].file_name[0])
		{
            if(valid_num == begno)
            {
                if(fileattr != NULL)
                {
                    memset(fileattr, 0x00, sizeof(file_attr_t));
                    sprintf(fileattr->filename, "/%d/%.16s", k_Flist[i].belong_app, k_Flist[i].file_name);
                    fileattr->belong_app = k_Flist[i].belong_app;
                    fileattr->file_len = k_Flist[i].size;
                }
                return(i);
            }
            valid_num++;
        }
    }
    return(-FS_NOTEXIST);
}

void fs_format(void)
{
	int32_t       i;
	int32_t       j;
	uint16_t      *tmpPtr;
#ifdef SRC_DEC_SPACE_CONFLIC
    uint16_t      buff[1024];
#else
    uint16_t      buff[4];
#endif
/*     int32_t       Col, Line; */
/*     uint8_t       CurSrData[2000]; */
    uint8_t       tempLog[10];

/*     lcd_store_screen((int *)&Col, (int *)&Line, CurSrData); */

	vCMD_OK = FILE_CMD_OK;
	vCMD_SET = FILE_CMD_SET;
	k_fsRecover = 0;
	s_fsprompt_begformat();
	k_FreeBlockPtr = SECTOR_BLOCK;
	k_ExchangeAddr = DATA_ADDR;
	k_FreeBlockNum = (DATA_SECTORS-1) * SECTOR_BLOCK;
	k_UsedBlockNum = 0;
	tmpPtr = &k_FAT[SECTOR_BLOCK];

	for(i=SECTOR_BLOCK+1; i<ALL_BLOCKS; i++)
	{
		*tmpPtr++ = (uint16_t)i;
	}

	*tmpPtr = NULL_BLOCK;
	memset((uint8_t *)(&k_Flist[0]), 0, MAX_FILES*sizeof(FILE_ITEM));
	for(i=0; i<MAX_FILES; i++)
	{
		k_Flist[i].belong_app = 0xfe;
	}

    __flash_read(tempLog, (uint8_t *)INIT_LOG_ADDR, 8);
	if(!memcmp(tempLog, "\xff\xff\xff\xff\xff\xff\xff\xff", 8))
	{
        memcpy(buff, FORMAT_FLAG, 8);
		__flash_write((uint16_t *)INIT_LOG_ADDR, buff, 4);
	}

    for (j = 0; j < FAT1_SECTORS; j++)
    {
        __flash_erase(FAT1_ADDR + j * FAT_SECTOR_SIZE);
    }
	s_fsprompt_erase(FAT1_ADDR);
    for (j = 0; j < FAT2_SECTORS; j++)
    {
        __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
    }
	s_fsprompt_erase(FAT2_ADDR);

	for(i=0; i<DATA_SECTORS; i++)
	{
		__flash_erase(DATA_ADDR + i*DATA_SECTOR_SIZE);
		s_fsprompt_erase(DATA_ADDR + i*DATA_SECTOR_SIZE);
	}

	for(i=0; i<3; i++)
	{
#ifdef SRC_DEC_SPACE_CONFLIC
        unsigned int wr_size;
        uint16_t *src_addr;
        uint16_t *des_addr;
        int continue_flag;
        unsigned int tmp_size;
#endif

		if(i)
		{
            for (j = 0; j < FAT1_SECTORS; j++)
            {
                __flash_erase(FAT1_ADDR + j * FAT_SECTOR_SIZE);
            }
            for (j = 0; j < FAT2_SECTORS; j++)
            {
                __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
            }
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+8), (uint16_t *)&k_ExchangeAddr, 2))
		{
			continue;
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+12), &k_FreeBlockNum, 1))
		{
			continue;
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+14), &k_UsedBlockNum, 1))
		{
			continue;
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+16), &k_FreeBlockPtr, 1))
		{
			continue;
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+18), k_FAT, ALL_BLOCKS))
		{
			continue;
		}
		if(__flash_write((uint16_t *)(FAT2_ADDR+ALL_BLOCKS*2+18), (uint16_t *)(&k_Flist[0]), (MAX_FILES*sizeof(FILE_ITEM))/2+1))
		{
			continue;
		}
		memcpy(buff, FAT_FLAG_VER, 8);
		if(__flash_write((uint16_t *)(FAT2_ADDR), buff, 4))
		{
			continue;
		}

#ifdef SRC_DEC_SPACE_CONFLIC
        wr_size = (FAT_SIZE - 8);
        src_addr = (uint16_t *)(FAT2_ADDR+8);
        des_addr = (uint16_t *)(FAT1_ADDR+8);
        continue_flag = 0;

        while (wr_size)
        {
            if (wr_size > sizeof(buff))
            {
                tmp_size = sizeof(buff);
            }
            else
            {
                tmp_size = wr_size;
            }
            __flash_read(buff, (uint8_t *)src_addr, tmp_size);
            if(__flash_write(des_addr, buff, tmp_size / 2))
            {
                continue_flag = 1;
                break;
            }
            wr_size -= tmp_size;
            src_addr += (tmp_size / 2);
            des_addr += (tmp_size / 2);
        }
        if (continue_flag)
        {
            continue;
        }
#else
//        if(__flash_write((uint16_t *)(FAT1_ADDR+8), (uint16_t *)(FAT2_ADDR+8), (FAT_SIZE-8)/2))
        if(__flash_writeEx((uint16_t *)(FAT1_ADDR+8), (uint16_t *)(FAT2_ADDR+8), (FAT_SIZE-8)/2))
        {
            continue;
        }
#endif
		memcpy(buff, FAT_FLAG_VER, 8);
		if(__flash_write((uint16_t *)(FAT1_ADDR), (uint16_t *)buff, 4))
		{
			continue;
		}
		break;
	}

	if(i == 3)
	{
		__flash_write_failed();
	}
	s_log_erase();
/*     lcd_restore_screen(Col, Line, CurSrData); */
}

static void s_resume_write_file(int32_t fd)
{
	int32_t           i;
	FILE_ITEM       *fItemPtr;
	FILE_STATUS     *fStatusPtr;
	uint16_t          newPtr;
	uint8_t           *chPtr;

	fItemPtr   = &k_Flist[fd];
	fStatusPtr = &k_Fstatus[fd];
	if((fStatusPtr->FilePtr == fItemPtr->size) && (fItemPtr->size%BLOCK_SIZE))
	{
		chPtr = (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr*BLOCK_SIZE + fStatusPtr->FilePtrOff);
		for(i = 0; i < BLOCK_SIZE-fStatusPtr->FilePtrOff; i++)
		{
            uint8_t temp_uint8;
            __flash_readb(&temp_uint8, (uint32_t)chPtr);
			if(temp_uint8 != 0xff)
			{
				break;
			}
		}

		if(i == BLOCK_SIZE - fStatusPtr->FilePtrOff)
		{
			return;
		}
		newPtr = s_get_free_block();
		k_FAT[newPtr] = NULL_BLOCK;

		if(fStatusPtr->UpBlockPtr == NULL_BLOCK)
		{
			fItemPtr->StartBlock = newPtr;
		}
		else
		{
			k_FAT[fStatusPtr->UpBlockPtr] = newPtr;
		}
		k_FAT[fStatusPtr->CurBlockPtr] = USED_BLOCK;
		k_UsedBlockNum++;
//		s_wr_block(newPtr, 0, (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr*BLOCK_SIZE), fStatusPtr->FilePtrOff);
		s_wr_blockEx(newPtr, 0, (uint8_t *)(DATA_ADDR + fStatusPtr->CurBlockPtr*BLOCK_SIZE), fStatusPtr->FilePtrOff);
	}
}

static void s_copy_sec(uint32_t sAddr)
{
	int32_t i;

	for(i=0; i<3; i++)
	{
		__flash_erase(sAddr);
		if(__flash_write((uint16_t *)sAddr, (uint16_t *)k_ExchangeAddr, DATA_SECTOR_SIZE/2))
		{
			continue;
		}
		break;
	}

	if(i == 3)
	{
		__flash_write_failed();
	}
}

static void s_load_fat(void)
{
	__flash_readl(&k_ExchangeAddr, FAT2_ADDR+8);
	__flash_readw(&k_FreeBlockNum, FAT2_ADDR+12);
	__flash_readw(&k_UsedBlockNum, FAT2_ADDR+14);
	__flash_readw(&k_FreeBlockPtr, FAT2_ADDR+16);
	__flash_read((uint8_t *)k_FAT,(uint8_t *)(FAT2_ADDR+18), ALL_BLOCKS*2);
	__flash_read((uint8_t *)(&k_Flist[0]), (uint8_t *)(FAT2_ADDR+ALL_BLOCKS*2+18), MAX_FILES*sizeof(FILE_ITEM));
	memset((uint8_t *)(&k_Fstatus[0]), 0, MAX_FILES*sizeof(FILE_STATUS));
}

void fs_init(void)
{
	int32_t   i;
	int32_t   j;
	uint32_t  len;
	uint8_t   tmpBuf[10];
	uint16_t  *tmpPtr, *oldPtr, *headPtr, fd, step, logSize, saveFAT;
//    uint32_t  BegTime, EndTime;
    uint8_t   tempVer[10];
    uint16_t temp_uint16;

/*     ALL_BLOCKS = DATA_SECTORS * SECTOR_BLOCK; */
/*     FAT_SIZE   = MAX_FILES*sizeof(FILE_ITEM) + ALL_BLOCKS*2 + 32; */

	vCMD_OK  = FILE_CMD_OK;
	vCMD_SET = FILE_CMD_SET;

#ifdef MAKE_CREATFILESYS
	fs_format();
#endif

    __flash_read(tempVer, (uint8_t *)FAT1_ADDR, 8);
	if(memcmp(FAT_FLAG_VER, tempVer, 8))
	{
        __flash_read(tempVer, (uint8_t *)FAT2_ADDR, 8);
		if(memcmp(FAT_FLAG_VER, tempVer, 8))
		{
			fs_format();
		}
		else if(__flash_readw(&temp_uint16, FAT1_LOG_ADDR), temp_uint16 == 0xffff)
		{
			fs_format();
		}
	}
    __flash_read(tempVer, (uint8_t *)INIT_LOG_ADDR, 8);
	if(!memcmp(FORMAT_FLAG, tempVer, 8))
	{
		fs_format();
	}
	k_fsRecover = 0;

    __flash_readw(&temp_uint16, LOG_ADDR);
    __flash_read(tempVer, (uint8_t *)(LOG_ADDR+LOG_SECTOR_SIZE-8), 8);
	if(temp_uint16 != 0xffff || memcmp(tempVer,LOG_FLAG_VER,8) != 0)
	{
		s_log_erase();
		s_load_fat();
		return;
	}

    __flash_readw(&temp_uint16, FAT1_LOG_ADDR);
	if(temp_uint16 == FILE_CMD_OK)
	{
        __flash_readw(&temp_uint16, FAT2_LOG_ADDR);
		if(temp_uint16 != FILE_CMD_OK)
		{
			for(i=0; i<3; i++)
			{
#ifdef SRC_DEC_SPACE_CONFLIC
                uint16_t  buff[1024];
                unsigned int wr_size;
                uint16_t *src_addr;
                uint16_t *des_addr;
                int continue_flag;
                unsigned int tmp_size;

                wr_size = (FAT_SIZE);
                src_addr = (uint16_t *)(FAT1_ADDR);
                des_addr = (uint16_t *)(FAT2_ADDR);
                continue_flag = 0;

                for (j = 0; j < FAT2_SECTORS; j++)
                {
                    __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
                }
                while (wr_size)
                {
                    if (wr_size > sizeof(buff))
                    {
                        tmp_size = sizeof(buff);
                    }
                    else
                    {
                        tmp_size = wr_size;
                    }
                    __flash_read(buff, (uint8_t *)src_addr, tmp_size);
                    if(__flash_write(des_addr, buff, tmp_size / 2))
                    {
                        continue_flag = 1;
                        break;
                    }
                    wr_size -= tmp_size;
                    src_addr += (tmp_size / 2);
                    des_addr += (tmp_size / 2);
                }
                if (continue_flag)
                {
                    continue;
                }

#else
                for (j = 0; j < FAT2_SECTORS; j++)
                {
                    __flash_erase(FAT2_ADDR + j * FAT_SECTOR_SIZE);
                }
//                if(__flash_write((uint16_t *)FAT2_ADDR, (uint16_t *)FAT1_ADDR, FAT_SIZE/2))
                if(__flash_writeEx((uint16_t *)FAT2_ADDR, (uint16_t *)FAT1_ADDR, FAT_SIZE/2))
                {
                    continue;
                }
#endif
				__flash_write((uint16_t *)FAT2_LOG_ADDR, &vCMD_OK, 1);
				break;
			}

			if(i == 3)
			{
				__flash_write_failed();
			}
		}

		s_log_erase();
		s_load_fat();
		return;
	}

	s_load_fat();

	k_LogPtr = (uint16_t *)(LOG_ADDR+16);
    __flash_readw(&temp_uint16, k_LogPtr);
	if(temp_uint16 == 0xffff)
	{
		return;
	}

	tmpPtr = LOG_END_ADDR + 20;
	while(tmpPtr > k_LogPtr)
	{
        __flash_readw(&temp_uint16, tmpPtr);
		if(temp_uint16 != 0xffff)
		{
			break;
		}
		tmpPtr--;
	}
	oldPtr  = k_LogPtr;
	headPtr = k_LogPtr;
	logSize = tmpPtr - k_LogPtr + 2;
	step    = logSize / 20;
	k_fsRecover = 1;

	s_fsprompt_percent(0);
    saveFAT = 0;
    /////////////
/*     BegTime = sys_get_counter(); */
	while(k_fsRecover)
	{
        __flash_readw(&temp_uint16, k_LogPtr);
		switch(temp_uint16)
		{
			case LOG_CREAT_FILE:
                __flash_readw(&temp_uint16, k_LogPtr+11);
				if(temp_uint16 == 0xffff)
				{
					saveFAT = 1;
					k_fsRecover = 0;
				}
				else
				{
                    uint16_t temp_uint16_2;
                    int8_t shortname[16];
                    __flash_readw(&temp_uint16, k_LogPtr+1);
                    __flash_readw(&temp_uint16_2, k_LogPtr+2);
                    __flash_read(shortname, (int8_t *)(k_LogPtr+3), 16);
					s_creat_file(temp_uint16, shortname, temp_uint16_2);
				}
                k_LogPtr += 12;
				break;

			case LOG_WRITE_FILE:
				__flash_readw(&fd, k_LogPtr+1);
				__flash_read(&k_Fstatus[fd], k_LogPtr, 18);
				k_LogPtr += 9;
                __flash_readw(&temp_uint16, k_LogPtr);
				if(temp_uint16 == 0xffff)
				{
					saveFAT = 1;
					k_fsRecover = 0;
					break;
				}
                __flash_readw(&temp_uint16, k_LogPtr);
				if(temp_uint16 != FILE_CMD_OK)
				{
					saveFAT = 1;
                    __flash_readw(&temp_uint16, k_LogPtr+1);
					if(temp_uint16 == LOG_FWRITE_FAILED)
					{
						while(__flash_readw(&temp_uint16, k_LogPtr+5), temp_uint16 == LOG_FWRITE_FAILED)
						{
							k_LogPtr += 4;
						}
                        __flash_readw(&temp_uint16, k_LogPtr+4);
						if(temp_uint16 == FILE_CMD_OK)
						{
							__flash_read(&len, k_LogPtr+2, 4);
							s_copy_sec(len);
						}
					}
					s_chk_used_block(k_Fstatus[fd].len/BLOCK_SIZE + 2);
					s_resume_write_file(fd);
					k_fsRecover = 0;
					break;
				}
				s_write_file(fd, tmpBuf);
				k_LogPtr++;
				break;

			case LOG_REMOVE_FILE:
                __flash_readw(&temp_uint16, k_LogPtr+2);
				if(temp_uint16 == 0xffff)
				{
					saveFAT     = 1;
					k_fsRecover = 0;
				}
				else
				{
                    __flash_readw(&temp_uint16, k_LogPtr+1);
					s_remove_by_id(temp_uint16);
				}
				k_LogPtr += 3;
				break;

			case LOG_TRUNCATE_FILE:
                __flash_readw(&temp_uint16, k_LogPtr+4);
				if(temp_uint16 == 0xffff)
				{
					saveFAT       = 1;
					k_fsRecover = 0;
					break;
				}
                __flash_readw(&temp_uint16, k_LogPtr+4);
				if(temp_uint16 != FILE_CMD_OK)
				{
					saveFAT = 1;
                    __flash_readw(&temp_uint16, k_LogPtr+5);
					if(temp_uint16 == LOG_FWRITE_FAILED)
					{
                        __flash_readw(&temp_uint16, k_LogPtr+8);
						if(temp_uint16 == FILE_CMD_OK)
						{
							__flash_read(&len, k_LogPtr+6, 4);
							s_copy_sec(len);
						}
					}
					s_chk_used_block(1);
					k_fsRecover = 0;
					break;
				}
				tmpPtr    = (uint16_t *)&len;
                __flash_readw(&temp_uint16, k_LogPtr+2);
				*tmpPtr++ = temp_uint16;
                __flash_readw(&temp_uint16, k_LogPtr+3);
				*tmpPtr   = temp_uint16;
                __flash_readw(&temp_uint16, k_LogPtr+1);
				s_truncate_file(temp_uint16, len);
				k_LogPtr += 5;
				break;

			case LOG_EXCHANGE_DATA:
                __flash_readw(&temp_uint16, k_LogPtr+3);
				if(temp_uint16 == 0xffff)
				{
					saveFAT       = 1;
					k_fsRecover = 0;
					break;
				}
				tmpPtr    = (uint16_t *)&len;
                __flash_readw(&temp_uint16, k_LogPtr+1);
				*tmpPtr++ = temp_uint16;
                __flash_readw(&temp_uint16, k_LogPtr+2);
				*tmpPtr   = temp_uint16;
				s_ex_page(len);
				k_LogPtr += 4;
				break;

			case LOG_RENAME_FILE:
                __flash_readw(&temp_uint16, k_LogPtr+11);
				if(temp_uint16 == 0xffff)
				{
					saveFAT     = 1;
					k_fsRecover = 0;
				}
				else
				{
                    uint16_t temp_uint16_2;
                    char shortname[16];
                    __flash_readw(&temp_uint16, k_LogPtr+1);
                    __flash_readw(&temp_uint16_2, k_LogPtr+2);
                    __flash_read(shortname, (char *)(k_LogPtr+3), 16);
					s_rename_by_id(temp_uint16, shortname, temp_uint16_2);
				}
				k_LogPtr += 12;
				break;

			default:
                __flash_readw(&temp_uint16, k_LogPtr);
				if(temp_uint16 == LOG_FWRITE_FAILED)
				{
					k_LogPtr += 4;
				}
				else
				{
					k_fsRecover = 0;
                    __flash_readw(&temp_uint16, k_LogPtr);
					if(temp_uint16 != 0xffff)
					{
						saveFAT = 1;
					}
				}
				break;
		}
		if(k_LogPtr-oldPtr > step)
		{
			oldPtr = k_LogPtr;
			s_fsprompt_percent((uint16_t)((k_LogPtr-headPtr)*100L/logSize));
		}
	}
	s_fsprompt_percent(100);

/*     EndTime = sys_get_counter(); */
/*     if(EndTime-BegTime >= 30000) */
/*     {                            */
/*         saveFAT = 1;             */
/*     }                            */

	if(saveFAT) s_save_fat();

	memset((uint8_t *)(&k_Fstatus[0]), 0, MAX_FILES*sizeof(FILE_STATUS));
}

#endif
