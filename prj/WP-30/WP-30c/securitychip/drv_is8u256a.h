#ifndef __DRV_IS8U256A_H__
#define __DRV_IS8U256A_H__

/**
 * \block:      Base defines
 * @{ */
/// 数据定义
//#define SC_HW_IS8U256A_SPI_ID   SPI_ID_2    //!< 
//#define SC_HW_IS8U256A_RST  PIOA_PIN_(12)   //!< 复位引脚
#define SC_IS8U_GET_STATUS  0xE5        //!< 查询芯片状态
#define SC_IS8U_STATUS_BUSY 0x00        //!< busy
#define SC_IS8U_STATUS_IDLE 0x01        //!< idle
#define SC_IS8U_WRITE       0xC1        //!< 发送
#define SC_IS8U_READ        0xA1        //!< 读取
#define SC_IS8U_DUMMY       0xFF        //!< 空字节，用于读取
#define SC_IS8U_APDU_CLA    0xBF        //!< SPI接口APDU指令集 - 头字符
/// 配置
#define SC_IS8U_BUFFER_SIZE (256 + 16)  //!< 每次收发数据的临时缓存大小
#define SC_SPI_BYTE_INTERVAL_US 120     //!< 字节间间隔(us)
                                        //!<    ! 延时不准确，需要设为100才能
                                        //!<    ! 正常通信
#define SC_IS8U_STATUS_WAIT_TIME 64     //!< 状态等待次数，每次等待100us

typedef enum {                          //!< 返回值
    SC_RET_SUCCESS          = 0,        //!< 成功
    SC_RET_PARAM_ERR        = -6901,    //!< 参数错误
    SC_IS8U_RET_BUSY        = -6902,    //!< 芯片忙
    SC_IS8U_RET_UNKNOWN_STATUS = -6903, //!< 芯片状态错误
    SC_IS8U_RET_SPI_ERR     = -6904,    //!< SPI收发错误
    SC_IS8U_RET_CLA_ERR     = -6905,    //!< CLA错误
    SC_IS8U_RET_LRC_ERR     = -6906,    //!< 异或值校验错误
    SC_IS8U_RET_INS_ERR     = -6907,    //!< INS错误
    SC_IS8U_RET_P1_2_ERR    = -6908,    //!< P1或P2错误
    SC_IS8U_RET_P3_ERR      = -6909,    //!< P3错误
    SC_IS8U_RET_RUN_ERR     = -6910,    //!< 函数功能错误，执行失败
    SC_IS8U_RET_OUT_OFS     = -6911,    //!< 操作地址越界
    SC_IS8U_RET_PROCESS_ERR = -6912,    //!< 流程错误
    SC_IS8U_RET_BAD_LRC     = -6913,    //!< 返回数据的异或值不正确
    SC_IS8U_RET_BAD_RESULT  = -6914,    //!< 未获取到正确结果
    SC_IS8U_RET_UNKNOWN     = -6915,    //!< 未知错误
}SC_IS8U_RET_E;
typedef struct {                        //!< SPI参数配置
    int spi_id;                         //!< SPI序号 0/1
    int spi_npcs;                       //!< NPCS
}SC_SPI_PARAMS_T;
typedef enum {                          //!< SM对称加解密模式
    SM_ECB_MODE,                        //!< ECB mode
    SM_CBC_MODE,                        //!< CBC mode
}SM_SYM_MODES_t;

/** @} */

//#if (defined(CFG_SECURITY_CHIP) && defined(CFG_SC_IS8U256A_SPI))
#if defined(CFG_SECURITY_CHIP)

/**
 * \block:      Base functions
 * @{ */
/**
 * \brief       芯片初始化
 * \return      0: Success  <0: Error
 */
int sc_spi_init(void);
/**
 * \brief       发送一个字节，并返回一个字节
 * \param       byte        发送字节数据
 * \return      返回字节数据
 */
unsigned char sc_spi_rw_byte(unsigned char byte);
/**
 * \brief       发送字符串，返回发送过程中读到的字符串
 * \param       data_in     发送数据
 * \param       size        发送数据长度
 * \param       data_out    返回数据
 * \return      >=0: 数据长度  <0: Error
 */
int sc_spi_rw(unsigned char *buf_in, int size, unsigned char *buf_out);

/**
 * \brief       获取芯片状态
 * \param       wait    0: 不等待空闲  ~: 等待空闲
 * \return      0: Idle     ~: Busy or Error
 */
int sc_is8u_spi_status(int wait);
/**
 * \brief       读取工厂码
 * \param       data        输出数据
 * \return      >=0: Success    <0: Error
 */
int sc_is8u_get_fac_code(unsigned char * data);
/** @} */
/**
 * \block:      IS8U256A特殊处理函数
 * @{ */
/**
 * \brief       IS8U256A初始化处理
 */
int sc_is8u256a_init(void);
/** @} */
/**
 * \block:      Crypto functions
 * @{ */
/**
 * \brief       获取随机数
 * \param       len         随机数长度
 * \param       data        随机数输出
 * \return      >=0: Length     <0: Error
 */
int sc_is8u_get_random_data(int len, unsigned char * data);
/**
 * \brief       读取对称算法和HASH算法运算结果
 * \param       len         读取长度
 * \param       data        读取数据缓存
 * \return      >=0: Length     <0: Error
 */
int sc_is8u_get_result_data(int len, unsigned char * data);
/**
 * \brief       SM1加密
 * \param       mode        加密模式，参考SM_SYM_MODES_t
 * \param       dat_in      明文数据
 * \param       len         数据长度 (len=16*n; n>0)
 * \param       key         密钥, 16 byte
 * \param       iv          初始化向量，CBC模式需要, 16 byte
 * \param       dat_out     输出数据
 * \return      0:Success   <0:Error
 */
int sc_is8u_sm1_enc(int mode, unsigned char * dat_in, int len,
        unsigned char * key, unsigned char * iv, unsigned char * dat_out);
/**
 * \brief       SM1解密
 * \param       mode        加密模式，参考SM_SYM_MODES_t
 * \param       dat_in      密文数据
 * \param       len         数据长度 (len=16*n; n>0)
 * \param       key         密钥, 16 byte
 * \param       iv          初始化向量，CBC模式需要, 16 byte
 * \param       dat_out     输出数据
 * \return      0:Success   <0:Error
 */
int sc_is8u_sm1_dec(int mode, unsigned char * dat_in, int len,
        unsigned char * key, unsigned char * iv, unsigned char * dat_out);


/**
 * \brief       生成密钥对
 * \return      0: Success  <0: Failed
 */
int sc_is8u_sm2_gen_keypair(void);
/**
 * \brief       导入密钥对
 * \param       pri_key     私钥 32 byte
 * \param       pub_key     公钥 64 byte
 * \return      0: Success  <0: Failed
 */
int sc_is8u_sm2_import_keypair(unsigned char * pri_key, unsigned char * pub_key);
/**
 * \brief       导出密钥对
 * \param       pri_key     私钥 32 byte
 * \param       pub_key     公钥 64 byte
 * \return      0: Success  <0: Failed
 */
int sc_is8u_sm2_export_keypair(unsigned char * pri_key, unsigned char * pub_key);
/**
 * \brief       SM2加密
 * \param       pt_len      明文长度
 * \param       pt          明文数据
 * \param       ct          输出密文数据缓存
 * \return      >0: 密文长度    <0: Failed
 */
int sc_is8u_sm2_enc(int pt_len, unsigned char * pt, unsigned char * ct);
/**
 * \brief       SM2解密
 * \param       ct_len      密文数据长度
 * \param       ct          密文数据
 * \param       pt          输出明文数据缓存
 * \return      >0: 明文长度    <0: Failed
 */
int sc_is8u_sm2_dec(int ct_len, unsigned char * ct, unsigned char * pt);
/**
 * \brief       SM2签名
 * \param       id_len      ID长度
 * \param       id          ID数据
 * \param       pt_len      明文数据长度
 * \param       pt          明文数据
 * \param       digest      摘要输出缓存
 * \param       sign        签名输出缓存
 * \return      >0: 签名数据长度  <0: Failed
 */
int sc_is8u_sm2_sign(int id_len, unsigned char * id,
        int pt_len, unsigned char * pt, unsigned char * sign);
/**
 * \brief       SM2验签 - 计算明文摘要值并验签
 * \param       id_len      id长度
 * \param       id          id数据
 * \param       pt_len      明文数据长度
 * \param       pt          明文数据
 * \param       sign        用于校验的签名数据
 * \return      0: Success  <0: Failed
 */
int sc_is8u_sm2_verify(int id_len, unsigned char * id,
        int pt_len, unsigned char * pt, unsigned char * sign);

/**
 * \brief       SHA1初始化
 * \return      0: Success  -1: Failed
 */
inline int sc_is8u_sha1_init(void);
/**
 * \brief       SHA1数据块运算
 * \param       len         输入数据长度，中间块大小必须等于0x40，末块小于0x40
 * \param       data        数据
 * \return      >=0: 数据长度   <0: Error
 */
inline int sc_is8u_sha1_update(int len, unsigned char * data);
/**
 * \brief       SHA1运算结束
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
inline int sc_is8u_sha1_finish(unsigned char * digest);
/**
 * \brief       SHA1
 * \param       len         输入数据长度
 * \param       data        数据
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
inline int sc_is8u_sha1(int len, unsigned char * data, unsigned char * digest);
/**
 * \brief       SHA256初始化
 * \return      0: Success  -1: Failed
 */
inline int sc_is8u_sha256_init(void);
/**
 * \brief       SHA256数据块运算
 * \param       len         输入数据长度，中间块大小必须等于0x40，末块小于0x40
 * \param       data        数据
 * \return      >=0: 数据长度   <0: Error
 */
inline int sc_is8u_sha256_update(int len, unsigned char * data);
/**
 * \brief       SHA256运算结束
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
inline int sc_is8u_sha256_finish(unsigned char * digest);
/**
 * \brief       SHA256
 * \param       len         输入数据长度
 * \param       data        数据
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
inline int sc_is8u_sha256(int len, unsigned char * data, unsigned char * digest);
/**
 * \brief       SM3初始化
 * \return      0: Success  -1: Failed
 */
inline int sc_is8u_sm3_init(void);
/**
 * \brief       SM3数据块运算
 * \param       len         输入数据长度，中间块大小必须等于0x40，末块小于0x40
 * \param       data        数据
 * \return      >=0: 数据长度   <0: Error
 */
inline int sc_is8u_sm3_update(int len, unsigned char * data);
/**
 * \brief       SM3运算结束
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
inline int sc_is8u_sm3_finish(unsigned char * digest);
/**
 * \brief       SM3
 * \param       len         输入数据长度
 * \param       data        数据
 * \param       digest      摘要结果输出缓存
 * \return      >=0: 摘要长度   <0: Error
 */
int sc_is8u_sm3(int len, unsigned char * data, unsigned char * digest);

/**
 * \brief       SM4 加密
 * \param       mode        加密模式, SM_SYM_MODES_t
 * \param       dat_in      明文
 * \param       len         数据长度 (len=16*n; n>0)
 * \param       key         密钥, 16 byte
 * \param       iv          初始化向量，CBC模式需要, 16 byte
 * \param       dat_out     密文
 * \return      0:Success   <0:Error
 */
int sc_is8u_sm4_enc(int mode, unsigned char * dat_in, int len,
        unsigned char * key, unsigned char * iv, unsigned char * dat_out);
/**
 * \brief       SM4 解密
 * \param       mode        加密模式, SM_SYM_MODES_t
 * \param       dat_in      明文
 * \param       len         数据长度 (len=16*n; n>0)
 * \param       key         密钥, 16 byte
 * \param       iv          初始化向量，CBC模式需要, 16 byte
 * \param       dat_out     密文
 * \return      0:Success   <0:Error
 */
int sc_is8u_sm4_dec(int mode, unsigned char * dat_in, int len,
        unsigned char * key, unsigned char * iv, unsigned char * dat_out);
/** @} */

#if defined(DEBUG_Dx)
int sc_is8u_test(void);
#endif /* defined(DEBUG_Dx) */
#endif /* (defined(CFG_SECURITY_CHIP) && defined(CFG_SC_IS8U256A_SPI)) */
#endif /* __DRV_IS8U256A_H__ */

