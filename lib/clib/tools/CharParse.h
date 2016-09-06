/*********************************************************************
******************************************************************************/



#define BCD2BIN(val)	(((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)	((((val)/10)<<4) + (val)%10)

unsigned char AscToHex(unsigned char *ucBuffer);

void HexToAsc(unsigned char ucData, unsigned char *pucDataOut);


int Str2Hex(int in_len, unsigned char *in, int *outlen, unsigned char *out);


int Hex2Str(int in_len, unsigned char *in, int *out_len, unsigned char *out);


long StrToLong(char *buff2);
 
//void LongToStr(long LongDat, char *buff2);

unsigned short StrToUshort(char *buff2);

void UshortToStr(unsigned short Num, char *buff2);

int UpperCase(char *sourceStr, char *destStr, int MaxLen);

void DeCompressToAscii(unsigned int uiLenIn, unsigned char * pucDataIn, unsigned char * pucDataOut);
uchar SI_ucParseStr(uchar ucMode,uchar ucLen,uchar *pcInStr,uchar *pcOutStr);
 
unsigned char MakeLrc(unsigned char *ucData, unsigned short usLen);
 
unsigned char CheckLrc(unsigned char *ucData, unsigned short usLen, unsigned char ucLrc);
 

void hexdump(unsigned char * b, unsigned short len);
 

void debug_hexdump(unsigned char * b, unsigned short len);
 

void Buf8ByteXor(unsigned int inlen, unsigned char *in, unsigned char *out);
 



void Str8ByteXor(unsigned int len, unsigned char *str1, unsigned char *str2, unsigned char *out);
 
//int mymemcmp(const void *s1, const void *s2, uint n);
 

unsigned int make_crc(unsigned int len, void *buf);
 
// mode=1  str tail add ":"
//     =2  str tail add "!"
void str2link(int mode, void *str1, void *str2, void *out);
 

// mode=1  str tail add ":"
//     =2  str tail add "!"
void str3link(int mode, void *str1, void *str2, void *str3, void *out);
 
int s_CheckStr (int inlen, unsigned char *in, int *outlen, unsigned char *out);


uint32_t msb_byte4_to_uint32(const uint8_t byte[4]);
uint16_t msb_byte2_to_uint16(const uint8_t byte[2]);
void msb_uint32_to_byte4(uint32_t byte4, uint8_t byte[4]);
void msb_uint16_to_byte2(uint16_t byte2, uint8_t byte[2]);
uint8_t ascii2_to_dec1(uint8_t ascii[2]);
uint32_t lsb_byte4_to_uint32(const uint8_t byte[4]);
void lsb_uint32_to_byte4(uint32_t byte4, uint8_t byte[4]);

