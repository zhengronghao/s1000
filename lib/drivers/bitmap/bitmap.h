#ifndef _BITMAP_H_
#define _BITMAP_H_

// Î»Í¼²ÎÊý
#define EM_BMP_TRANSPARENCE             0x00
#define EM_BMP_NOT_TRANSPARENCE         0x01

#define EM_BMP_ROTATE0                  0x00
#define EM_BMP_ROTATE90                 0x01
#define EM_BMP_ROTATE180                0x02
#define EM_BMP_ROTATE270                0x03

#define RGB(r,g,b)          ((uchar)~((r+g+b)/3))

#define BMP_FILE_FLAGE		"BM"

//#ifdef __ICCARM__						// IAR
//#pragma pack(1)            // IAR
//#define __attribute__(...) // IAR
//#endif	
typedef __packed struct { // bmfh
    uint16_t		wBmpFlag;
    uint32_t		bfSize;
    uint16_t		bfReserved1;
    uint16_t		bfReserved2;
    uint32_t		bfOffBits;

    uint32_t		biSize;
    int32_t			biWidth;
    int32_t			biHeight;
    uint16_t		biPlanes;
    uint16_t		biBitCount;
    uint32_t		biCompression;
    uint32_t		biSizeImage;
    int32_t			biXPelsPerMeter;
    int32_t			biYPelsPerMeter;
    uint32_t		biClrUsed;
    uint32_t		biClrImportant;
}BITMAP_INFO_HEADER;

typedef __packed struct { // rgbq
    uint8_t			rgbBlue;
    uint8_t			rgbGreen;
    uint8_t			rgbRed;
    uint8_t			rgbReserved;
} RGB_QUAD;

typedef __packed struct {
	BITMAP_INFO_HEADER	bmiHeader;
	RGB_QUAD			bmiColors[0];
}BITMAPINFO;

//#ifdef __ICCARM__          // IAR
//#pragma pack()             // IAR
//#endif  




extern const RGB_QUAD black_white_CLT[];

void encoding8to1(const void *src, uint32_t size, void *des);
void encodeing1to1(const void *src, uint32_t size, void *des);
int encode2bmp(const void *image, uint32_t width, uint32_t height,
					const RGB_QUAD clt[], uint32_t cltnum,
					uint32_t bitperpixel, uint32_t bytesperline,
					void (*encode_a_line)(const void * src, uint32_t size, void * des),
					void *bmpfilebuffer);


uchar s_GetBitmapInfo(const void *pvBitmapSrc, uint *puiWidth, uint *puiHigh, uint *puiColor);
int s_DrawBitmap(int iStartX, int iStartY, uchar ucType,uchar ucTransparent, 
					const void *pvBitmapSrc, int (*Paint)(int X, int Y, int Color));
int sys_encode2bmp(const void *image, uint width, uint height, void *bmpbuf);


#endif
