#include "common.h"
#include "bitmap.h"

const RGB_QUAD black_white_CLT[] = {
	[0] = {0x00, 0x00, 0x00, 0x00},
	[1] = {0xFF, 0xFF, 0xFF, 0x00},
};

void encoding8to1(const void *src, uint32_t size, void *des)
{
	const uint8_t *source = (uint8_t *)src;
	uint8_t *encodebuf = (uint8_t *)des;
	uint8_t data;
	int32_t i, k, length;

	for (i = 0; i < size; i += 8) {
		length = size - i > 8 ? 8 : size - i;
		for (k = 0, data = 0; k < length; k++) {
			data |= (source[i+k] & 1) << (7 - k);
		}
		*encodebuf++ = ~data;
	}
}

void encodeing1to1(const void *src, uint32_t size, void *des)
{
	const uint8_t *source = (uint8_t *)src;
	uint8_t *encodebuf = (uint8_t *)des;
	int32_t length = (size + 3) / 4 * 4, i;

	for (i = 0; i < size; i++)
	{
		encodebuf[i] = ~source[i];
	}

	memset(&encodebuf[size], 0x00, length - size);
}

int encode2bmp(const void *image, uint32_t width, uint32_t height,
					const RGB_QUAD clt[], uint32_t cltnum,
					uint32_t bitperpixel, uint32_t bytesperline,
					void (*encode_a_line)(const void * src, uint32_t size, void * des),
					void *bmpfilebuffer)
{

	BITMAPINFO bmpinfo;
	int32_t DataSizePerLine, DataSize, k;
	uint8_t	*buffer = (uint8_t *)bmpfilebuffer, *src;

   // 计算扫描一行需要字节数
   DataSizePerLine= ((width * bitperpixel + 3 )/8 + 3)/ 4 * 4;
   DataSize = DataSizePerLine * height;

	memset(&bmpinfo, 0x00, sizeof(bmpinfo));

	/* 设置位图标志 */
	memcpy(&bmpinfo.bmiHeader.wBmpFlag, BMP_FILE_FLAGE, sizeof(BMP_FILE_FLAGE));

	/* 计算文件大小 */
	bmpinfo.bmiHeader.bfSize = 	sizeof(bmpinfo.bmiHeader) + /* 结构头 */
								sizeof(clt[0]) * cltnum +	/* 彩色表 */
								DataSize;	/* 数据 */
	/* 计算数据相对文件的偏移 */
	bmpinfo.bmiHeader.bfOffBits = bmpinfo.bmiHeader.bfSize - DataSize;

	/* 计算位图头大小 */
	bmpinfo.bmiHeader.biSize = 0x28;

	/* 设置基本信息 */
	bmpinfo.bmiHeader.biWidth			= width;
	bmpinfo.bmiHeader.biHeight			= height;
	bmpinfo.bmiHeader.biPlanes			= 1;
	bmpinfo.bmiHeader.biBitCount		= bitperpixel;
	bmpinfo.bmiHeader.biCompression		= 0; 			/* 无压缩 */
	bmpinfo.bmiHeader.biSizeImage		= DataSize;
	bmpinfo.bmiHeader.biXPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biYPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biClrUsed			= cltnum;
	bmpinfo.bmiHeader.biClrImportant	= 0;

	memcpy(buffer, &bmpinfo, sizeof(bmpinfo));
	buffer += sizeof(bmpinfo);

	/* 填充彩色表 */
	memcpy(buffer, clt, sizeof(RGB_QUAD) * cltnum);
	buffer += sizeof(RGB_QUAD) * cltnum;
	/* 填充数据 */
	for (k = 0; k < height; k++) {
		src = (uint8_t *)((unsigned long)image + bytesperline * (height - 1 - k));
		encode_a_line(src, width*bitperpixel/8, buffer);
		buffer += DataSizePerLine;
	}

	return (int)((unsigned long)buffer - (unsigned long)bmpfilebuffer);
}



//	 获取位图信息
uint8_t s_GetBitmapInfo(const void *pvBitmapSrc, uint *puiWidth, uint *puiHigh, uint *puiColor)
{
    BITMAP_INFO_HEADER BitmapInfoHead;

    // 检查是否为bmp文件
    if (memcmp(pvBitmapSrc, "BM", sizeof(uint16_t)) != 0)
    {
        return NO;
    }

    // 拷贝位图文件头
    memcpy(&BitmapInfoHead, (void*)((unsigned long)pvBitmapSrc), sizeof(BITMAP_INFO_HEADER));
    *puiWidth = BitmapInfoHead.biWidth;
    *puiHigh  = BitmapInfoHead.biHeight;
    *puiColor = 1 << BitmapInfoHead.biBitCount;
    return YES;
}

// 画位图，位图资源就是位图文件数据
int s_DrawBitmap(int iStartX, int iStartY, uint8_t ucType,uint8_t ucTransparent,
                                const void *pvBitmapSrc,
                                int (*Paint)(int X, int Y, int Color))
{
    BITMAP_INFO_HEADER const *pBitmapInfoHead = (BITMAP_INFO_HEADER *)pvBitmapSrc;
//    RGB_QUAD           RgbQuad[256];
    RGB_QUAD const * pRgbQuad = (RGB_QUAD *)((uint8_t *)pvBitmapSrc+sizeof(BITMAP_INFO_HEADER));
    uint8_t            *pbBitmapPointer = NULL;
    uint8_t		bMask;
//    int32_t		RGBCount;
    int32_t		DataSizePerLine;
    int32_t		iColor, iBits;
    int32_t		iLineBytes, count;
    int32_t		i, k, n;
    int32_t		x, y, Colors;
    int32_t   	FillHeight = 0;
    int32_t   	iRet;

    if (!pvBitmapSrc)
    {
        return(FillHeight);
    }
    // 检查是否为bmp文件
    if (memcmp(pvBitmapSrc, "BM", sizeof(uint16_t)) != 0)
    {
        return(FillHeight);
    }

    // 拷贝位图文件头
//    memcpy(&BitmapInfoHead, (void*)((unsigned long)pvBitmapSrc), sizeof(BITMAP_INFO_HEADER));
//    pBitmapInfoHead = (BITMAP_INFO_HEADER *)pvBitmapSrc;

    // 拷贝颜色标数据
//    if (pBitmapInfoHead->biBitCount <= 8)
//    {
//        RGBCount = (1<<pBitmapInfoHead->biBitCount) & 0x00FF;
//        memcpy(RgbQuad, (void*)((unsigned long)pvBitmapSrc+sizeof(BITMAP_INFO_HEADER)), RGBCount*sizeof(RGB_QUAD));
//    }

    // 计算扫描一行需要字节数
    DataSizePerLine= (pBitmapInfoHead->biWidth * pBitmapInfoHead->biBitCount+31)/8;

    // 计算存储一行需要的字节数，是4的倍数
    iLineBytes = DataSizePerLine/4*4;

    // 不支持压缩位图
    if (pBitmapInfoHead->biCompression != 0)
    {
        return(FillHeight);
    }

    // 画位图
    switch(pBitmapInfoHead->biBitCount)
    {
    case 1:     // 单色位图
    case 4:     // 16色位图
    case 8:     // 256色位图
        iBits = 8 / pBitmapInfoHead->biBitCount;
        bMask = (uint8_t)(0xFF >> (8-pBitmapInfoHead->biBitCount));
        for(i=0; i < pBitmapInfoHead->biHeight; i++) // 扫描高度
        {
            // 位图数据是从文件尾开始扫描的
            pbBitmapPointer = (uint8_t*)((uint32_t)pvBitmapSrc + pBitmapInfoHead->bfSize - (i+1) *iLineBytes);
            for(k=0, count = 0; k < DataSizePerLine; k++) // 扫描一行
            {
                for(n=0; n < iBits && count < pBitmapInfoHead->biWidth; n++, count++)
                {
                    iColor = (pbBitmapPointer[k] >> (8- (n+1)*pBitmapInfoHead->biBitCount)) & bMask;
                    switch(ucType) // 位图旋转
                    {
                    case EM_BMP_ROTATE90:
                        y =  k*iBits+n;
                        x =  pBitmapInfoHead->biHeight - i;
                        break;
                    case EM_BMP_ROTATE180:
                        x = pBitmapInfoHead->biWidth - (k*iBits+n);
                        y = pBitmapInfoHead->biHeight - i;
                        break;
                    case EM_BMP_ROTATE270:
                        y =  pBitmapInfoHead->biWidth - (k*iBits+n);
                        x =  i;
                        break;
                    default:
                        x = k*iBits+n;
                        y = i;
                        break;
                    }
                    Colors = RGB(pRgbQuad[iColor].rgbRed, pRgbQuad[iColor].rgbGreen, pRgbQuad[iColor].rgbBlue);
                    if (ucTransparent != EM_BMP_TRANSPARENCE) // 不透明处理
                    {
                        iRet = Paint(iStartX+x, iStartY+y, Colors);
                        if(iRet == OK)
                        {
                            FillHeight = MAX(FillHeight, y+1);
                        }
                    }
                }
            }
        }
        break;
    case 24:    // 24位真彩色位图
        for(i=0; i < pBitmapInfoHead->biHeight; i++)
        {
            for(n=0; n < pBitmapInfoHead->biWidth; n++)
            {
                switch(ucType)  // 位图旋转
                {
                case EM_BMP_ROTATE90:
                    y =  n;
                    x =  pBitmapInfoHead->biHeight - i;
                    break;
                case EM_BMP_ROTATE180:
                    x = pBitmapInfoHead->biWidth - n;
                    y = pBitmapInfoHead->biHeight - i;
                    break;
                case EM_BMP_ROTATE270:
                    y =  pBitmapInfoHead->biWidth - n;
                    x =  i;
                    break;
                default:
                    x = n;
                    y = i;
                    break;
                }
                k = n*3;
                Colors = RGB(pbBitmapPointer[k+2], pbBitmapPointer[k+1], pbBitmapPointer[k+0]);
                if(ucTransparent != EM_BMP_TRANSPARENCE)   // 不透明处理
                {
                    iRet = Paint(iStartX+x, iStartY+y, Colors);
                    if(iRet == OK)
                    {
                        FillHeight = MAX(FillHeight, y+1);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    return(FillHeight);
}

// 将输入数据转换成BMP格式文件
int sys_encode2bmp(const void *image, uint width, uint height, void *bmpbuf)
{
    if(image != NULL && bmpbuf != NULL)
    {
        return encode2bmp(image, width, height, black_white_CLT,2,1,(width+7)/8, encodeing1to1,bmpbuf);
    }
    else
    {
        return 0;
    }
}

