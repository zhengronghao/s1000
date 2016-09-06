/**Copyright(C) For free. All right reserved.
 */
/**
 * \file        debug.h
 * \brief       调试模块
 * \author      huanglf
 * \version     1.4
 * \date        2015-10-01
 */


#ifndef __DEBUG_MODULE_H__
#define __DEBUG_MODULE_H__

#ifndef DS_DEBUG_MAIN
#define DS_DEBUG_MAIN       0           //!< 调试模块总开关，默认关闭
#endif /* DS_DEBUG_MAIN */

/// debug模块功能配置
#define DBG_COLOR_EN        1           //!< 使能带颜色输出
#define DBG_INPUT_EN        1           //!< 使能输入功能
#define DBG_DUMP_EN         1           //!< 使能数据导出功能
#define DBG_LOG_EN          0           //!< 使能日志功能
#define DBG_NL_HEAD         1           //!< 换行符放在开头
#define DBG_NL_CHAR         "\r\n"        //!< 换行符
#define DBG_BUFFER_SIZE     4096        //!< 输入输出数据处理缓存大小

typedef enum {                          //!< 显示模式选项 - 前8位为通用定义
    DBG_INFO                = 1 << 0,   //!< 显示输出信息
    DBG_LABEL_TIME          = 1 << 1,   //!< 标签显示时间
    DBG_LABEL_FILE          = 1 << 2,   //!< 标签显示文件
    DBG_LABEL_FUNC          = 1 << 3,   //!< 标签显示函数
    DBG_LABEL_LINE          = 1 << 4,   //!< 标签显示行号
    DBG_NO_DUMP             = 1 << 5,   //!< 不显示Dump数据
    DBG_UNLOG               = 1 << 6,   //!< 不写入日志
}DBG_MODE_SWITCH_OPT_t;
typedef enum {                          //!< 返回值
    DBG_RET_OK              = 0,        //!< 成功
    DBG_RET_PARAM_ERR       = -11,      //!< 参数错误
    DBG_RET_OPEN_FILE_ERR   = -12,      //!< 打开文件错误
    DBG_RET_CLOSE_FILE_ERR  = -13,      //!< 关闭文件错误
    DBG_RET_WRITE_FILE_ERR  = -14,      //!< 写文件错误
    DBG_RET_NO_INPUT        = -15,      //!< 无输入
    DBG_RET_UNKNOWN_INPUT   = -16,      //!< 未知输入
}DBG_RET_t;
typedef enum {                          //!< Debug模块调试开关
    DS_OUT_ERR  = (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE), //!< 输出错误
    DS_IN_ERR   = (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE), //!< 输入错误
    DS_LOG_ERR  = (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE), //!< 日志错误
    DS_DUMP_ERR = (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE), //!< 数据导出错误
    DS_TEST_ERR = (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE), //!< 测试错误
}DS_DEBUG_MODULE_t;

/**
 * \block:      Base Input/Output Interface
 * @{ */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief       输出接口函数类型
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0: Success      <0: Error
 */
typedef int (* DBG_FUNC_OUTPUT_T)(char * buf, int len);
/**
 * \brief       输入接口函数类型
 * \param       buf         获取输入数据的缓存
 * \param       len         缓存大小
 * \return      0: Success      <0: Error
 * \detail      输入过程不立即返回，直到收到回车才退出
 */
typedef int (* DBG_FUNC_INPUT_T)(char * buf, int len);
/**
 * \brief       打开文件接口函数类型
 * \param       filename    文件名
 * \return      0: Success      <0: Error
 * \detail      必须以可写、追加模式打开文件。
 *              或者打开后必须将读写指针移到文件末尾。
 *              打开文件接口函数返回的指针或句柄，由接口内部管理。
 */
typedef int (* DBG_FUNC_FOPEN_T)(char * filename);
/**
 * \brief       关闭文件接口函数类型
 * \return      0: Success      <0: Error
 * \detail      如未打开日志，需返回0
 */
typedef int (* DBG_FUNC_FCLOSE_T)(void);
/**
 * \brief       写文件接口函数类型
 * \param       buf         写入的数据
 * \param       len         写入数据的长度
 * \return      0: Success      <0: Error
 * \detail      如未打开日志，需返回0
 */
typedef int (* DBG_FUNC_FWRITE_T)(char * buf, int len);
/**
 * \brief       同步文件接口函数类型
 * \return      0: Success      <0: Error
 * \detail      如未打开日志，需返回0
 */
typedef int (* DBG_FUNC_FSYNC_T)(void);
typedef struct {                        //!< 基本输入输出接口配置结构体
    DBG_FUNC_OUTPUT_T f_output;         //!< 输出接口函数
    DBG_FUNC_INPUT_T f_input;           //!< 输入接口函数
    DBG_FUNC_FOPEN_T f_open;            //!< 打开文件接口函数
    DBG_FUNC_FCLOSE_T f_close;          //!< 关闭文件接口函数
    DBG_FUNC_FWRITE_T f_write;          //!< 写文件接口函数
    DBG_FUNC_FSYNC_T f_sync;            //!< 同步文件接口函数
}DBG_BIO_T;

#if (DS_DEBUG_MAIN == 1)
/**
 * \brief       输出接口
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_bio_out(char * buf, int len);
/**
 * \brief       输入接口
 * \param       buf         获取输入数据的缓存
 * \param       len         缓存大小
 * \return      0: Success      <0: DBG_RET_t
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int dbg_bio_in(char * buf, int len);
#if (DBG_LOG_EN == 1)
/**
 * \brief       打开日志文件
 * \param       file        文件名
 * \return      0: Success      <0: DBG_RET_t
 * \detail      打开新日志时自动关闭旧的日志文件
 *              使用追加模式打开日志，不存在则创建
 */
int dbg_bio_open(char * file);
/**
 * \brief       关闭日志文件
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_bio_close(void);
/**
 * \brief       写日志
 * \param       buf         写入数据缓存
 * \param       len         写入数据长度
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_bio_write(char * buf, int len);
/**
 * \brief       同步数据，将缓存中的数据保存到存储设备
 * \return      0: Success      <0: DBG_RET_t
 * \detail      同步数据较慢，可能影响调试输出速度
 */
int dbg_bio_sync(void);
#endif /* (DBG_LOG_EN == 1) */
/**
 * \brief       配置基本接口
 * \param       bio         接口配置结构体指针, NULL:使用默认接口
 * \return      0: Success      <0: DBG_RET_t
 * \detail      切换接口时会先自动关闭日志文件。
 */
int dbg_bio_conf(DBG_BIO_T * bio);
#else
#define dbg_bio_conf(...)
#endif /* (DS_DEBUG_MAIN == 1) */
#ifdef __cplusplus
}
#endif
/** @} */
/**
 * \block:      STDOUT
 * @{ */
/// 输出颜色定义
#define DBG_COLOR_RES   "\33[0m"        //!< 恢复
#define DBG_COLOR_INFO  "\33[1m"        //!< 提示高亮
#define DBG_COLOR_WARN  "\33[1;36m"     //!< 警告高亮
#define DBG_COLOR_ERR   "\33[1;91m"     //!< 错误高亮
#define DBG_COLOR_HL    "\33[7m"        //!< 反白高亮
#define DBG_COLOR_INPUT "\33[1;92m"     //!< 输入标志高亮
// 标记定义
#define DBG_MODE_SIGN_INFO      ""      //!< 提示标记
#define DBG_MODE_SIGN_WARN      "[Warning]" //!< 警告标记
#define DBG_MODE_SIGN_ERR       "[Error]" //!< 错误标记
#define DBG_MODE_SIGN_IN_RETNUM "[I_n]" //!< 返回数值输入标记
#define DBG_MODE_SIGN_IN_GETNUM "[I_n]" //!< 获取数组输入标记
#define DBG_MODE_SIGN_IN_GETSTR "[I_s]" //!< 获取字符串输入标记
#define DBG_MODE_SIGN_IN_ECHO   "[O]"   //!< 返回数值输出标记

// 输出标签模式选项定义
typedef enum {
    // Debug output defines
    DBG_LABEL_STDERR    = 1 << 8,       //!< 显示错误信息
    DBG_LABEL_NEWLINE   = 1 << 9,       //!< 添加换行符
    DBG_LABEL_INFO      = 1 << 10,      //!< 添加提示标记
    DBG_LABEL_WARN      = 1 << 11,      //!< 添加警告标记
    DBG_LABEL_ERR       = 1 << 12,      //!< 添加错误标记
    DBG_LABEL_IN_RETN   = 1 << 13,      //!< 添加数字输入标记
    DBG_LABEL_IN_GETN   = 1 << 14,      //!< 添加数值输入标记
    DBG_LABEL_IN_GETS   = 1 << 15,      //!< 添加字符串输入标记
    DBG_LABEL_IN_ECHO   = 1 << 16,      //!< 添加输入回显标记
    DBG_LABEL_COL_INFO  = 1 << 17,      //!< 显示信息使用提示高亮
    DBG_LABEL_COL_WARN  = 1 << 18,      //!< 显示信息使用警告高亮
    DBG_LABEL_COL_ERR   = 1 << 19,      //!< 显示信息使用错误高亮
    DBG_LABEL_COL_HL    = 1 << 20,      //!< 显示信息使用反白高亮
}DBG_LABEL_MODE_t;

#if (DS_DEBUG_MAIN == 1)
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief       格式化输出
 * \param       opt         参考宏定义DBG_LABEL_*
 * \param       fmt         格式字符串，内部缓存大小为DBG_BUFFER_SIZE
 * \param       ...         格式化参数
 * \return      0: Success          <0: DBG_RET_t
 */
int dbg_stdout(int opt, const char * fmt, ...);
/**
 * \brief       标准错误信息输出
 * \param       opt         参考宏定义DBG_LABEL_*
 * \return      0: Success          <0: DBG_RET_t
 */
int dbg_stderr(int opt);
#if (DBG_COLOR_EN == 1)
/**
 * \brief       设置终端输出颜色
 * \param       color       颜色标示
 * \return      0: Success          <0: DBG_RET_t
 */
int dbg_color_set(char * color);
#endif /* (DBG_COLOR_EN == 1) */
/**
 * \brief       带调试标签的格式化输出
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       opt         参考宏定义DBG_LABEL_*
 * \param       fmt         格式化输出字符串
 * \param       ...         格式化参数
 * \return      0: Success          <0: DBG_RET_t
 */
int dbg_stdout_label(const char * file, const char * func, int line,
        int opt, const char * fmt, ...);
#ifdef __cplusplus
}
#endif

/** \brief      格式化输出 */
#define dbg_out(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout(__debug_switch__, __VA_ARGS__); \
    } \
}
/** \brief      带提示标签的格式化输出，标签为提示高亮 */
#define dbg_out_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_INFO | DBG_LABEL_NEWLINE,\
                __VA_ARGS__); \
    } \
}
/** \brief      带警告标签的格式化输出，标签为警告高亮 */
#define dbg_out_W(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_WARN | DBG_LABEL_NEWLINE,\
                __VA_ARGS__); \
    } \
}
/** \brief      带错误标签的格式化输出，标签为错误高亮 */
#define dbg_out_E(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_ERR | DBG_LABEL_NEWLINE,\
                __VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为高亮 */
#define dbg_out_H(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_COL_HL | DBG_LABEL_NEWLINE,\
                __VA_ARGS__); \
    } \
}
/** \brief      带错误标签的格式化输出，标签为提示高亮，附带错误信息 */
#define dbg_outerr_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_INFO | DBG_LABEL_STDERR\
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带错误标签的格式化输出，标签为警告高亮，附带错误信息 */
#define dbg_outerr_W(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_WARN | DBG_LABEL_STDERR\
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带错误标签的格式化输出，标签为错误高亮，附带错误信息 */
#define dbg_outerr_E(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_ERR | DBG_LABEL_STDERR\
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      函数进入标志 */
#define dbg_out_entry(__debug_switch__) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_COL_INFO | DBG_LABEL_NEWLINE,\
                ">>> %s {", __func__); \
    } \
}
/** \brief      函数退出标志 */
#define dbg_out_exit(__debug_switch__) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__FILE__, __func__, __LINE__, \
                __debug_switch__ | DBG_LABEL_COL_INFO | DBG_LABEL_NEWLINE,\
                "<<< %s }", __func__); \
    } \
}
#else
#define dbg_stdout(...)
#define dbg_stderr(...)
#define dbg_color_set(...)
#define dbg_stdout_label(...)

#define dbg_out(...)
#define dbg_out_I(...)
#define dbg_out_W(...)
#define dbg_out_E(...)
#define dbg_out_H(...)
#define dbg_outerr_I(...)
#define dbg_outerr_W(...)
#define dbg_outerr_E(...)
#define dbg_out_entry(...)
#define dbg_out_exit(...)
#endif /* (DS_DEBUG_MAIN == 1) */
/** @} */
/**
 * \block:      STDIN
 * @{ */
typedef enum {                          //!< 输入模式
    DBG_MODE_STDIN_RETNUM   = 1,        //!< 返回输入的数值
    DBG_MODE_STDIN_GETNUM,              //!< 获取输入的数值
    DBG_MODE_STDIN_GETSTR,              //!< 获取输入的字符串
}DBG_MODE_STDIN_MODE_t;

#if ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1))
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief       获取输入的字符串
 * \param       str         字符串缓存
 * \param       len         缓存大小
 * \return      >=0: 获取的数据长度 <0: DBG_RET_t
 * \detail      自动移除末尾换行符
 */
int dbg_stdin(char * str, int len);
/**
 * \brief       获取输入的数值
 * \param       num         变量指针，输出获取到的数值
 * \return      0: Success      <0: DBG_RET_t
 * \detail      0x开头识别为16进制，0开头识别为8进制
 */
int dbg_stdin_num(int * num);
/**
 * \brief       带调试标签的输入
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       mode        输入模式，DBG_MODE_STDIN_MODE_t
 * \param       output      DBG_MODE_STDIN_GETSTR：输出数据的缓存
 *                          DBG_MODE_STDIN_GETNUM：整形指针，用于获取输入数值
 *                          DBG_MODE_STDIN_RETNUM：不使用，NULL
 * \param       len         DBG_MODE_STDIN_GETSTR：获取的数据长度
 *                          DBG_MODE_STDIN_GETNUM：不使用
 *                          DBG_MODE_STDIN_RETNUM：不使用
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_stdin_label(const char * file, const char * func, int line,
        int mode, void * output, int len);
#ifdef __cplusplus
}
#endif

/** \brief      返回输入的数值，如果输入值不可转换为数字则返回-1 */
#define dbg_in() ({ \
        dbg_stdin_label(__FILE__, __func__, __LINE__, \
                DBG_MODE_STDIN_RETNUM, 0, 0); \
        })
/** \brief      获取输入的数值，可识别8进制、10进制和16进制 */
#define dbg_in_N(pnum) ({ \
        dbg_stdin_label(__FILE__, __func__, __LINE__, \
                DBG_MODE_STDIN_GETNUM, pnum, 0); \
        })
/** \brief      获取输入的字符串，移除换行符 */
#define dbg_in_S(pstr, len) ({ \
        dbg_stdin_label(__FILE__, __func__, __LINE__, \
                DBG_MODE_STDIN_GETSTR, pstr, len); \
        })
#else
#define dbg_stdin(...)
#define dbg_stdin_num(...)
#define dbg_stdin_label(...)

#define dbg_in() 0
#define dbg_in_N(...)
#define dbg_in_S(...)
#endif /* ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1)) */
/** @} */
/**
 * \block:      LOG
 * @{ */
typedef enum {                          //!< Output mode
    DBG_MODE_P  = 1 << 0,               //!< Print only
    DBG_MODE_W  = 1 << 1,               //!< Write log only
    DBG_MODE_PW = DBG_MODE_P | DBG_MODE_W, //!< Both print and write log
    DBG_MODE_S  = 1 << 2,               //!< Write log by sync mode
}DBG_MODE_t;

#if ((DBG_LOG_EN == 1) && (DS_DEBUG_MAIN == 1))
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief       打开新日志
 * \param       filename    日志文件名
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_log_open(char * filename);
/**
 * \brief       关闭日志
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_log_close(void);
/**
 * \brief       写日志
 * \param       buf     数据
 * \param       len     数据长度
 * \return      >=0: 写入数据长度   <0: DBG_RET_t
 * \detail      不输出
 */
int dbg_log_write(char * buf, int len);
/**
 * \brief       设置日志记录模式
 * \param       mode    调试模式 dbg_modes_e
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_log_setmode(int mode);
/**
 * \brief       获取日志记录模式
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_log_getmode(void);
#ifdef __cplusplus
}
#endif

/** \brief      设置日志文件名 */
#define dbg_log_setname(filename) { \
    dbg_log_open(filename); \
}
/** \brief      关闭日志调试 */
#define dbg_log_off() { \
    dbg_log_setmode(DBG_MODE_P); \
}
/** \brief      打开日志调试 */
#define dbg_log_on() { \
    dbg_log_setmode(DBG_MODE_PW); \
}
/** \brief      关闭输出，打开日志调试 */
#define dbg_log_only() { \
    dbg_log_setmode(DBG_MODE_W); \
}
/** \brief      开启日志调试，同步写入 */
#define dbg_log_on_s() { \
    dbg_log_setmode(DBG_MODE_PW | DBG_MODE_S); \
}
/** \brief      关闭输出，打开日志调试，同步写入 */
#define dbg_log_only_s() { \
    dbg_log_setmode(DBG_MODE_W | DBG_MODE_S); \
}
#else
#define dbg_log_open(...)
#define dbg_log_close()
#define dbg_log_write(...)
#define dbg_log_setmode(...)
#define dbg_log_getmode()

#define dbg_log_setname(...) { \
    dbg_out_E(1, "DBG_LOG_EN is OFF!"); \
}
#define dbg_log_off()
#define dbg_log_on()
#define dbg_log_only()
#define dbg_log_on_s()
#define dbg_log_only_s()
#endif /* ((DBG_LOG_EN == 1) && (DS_DEBUG_MAIN == 1)) */
/** @} */
/**
 * \block:      DUMP
 * @{ */
#ifdef __cplusplus
extern "C" {
#endif
// Dump defines
#define DBG_DMP_DAT_NO      ( 0 << 8 )  //!< 不显示数据
#define DBG_DMP_DAT_HEX     ( 1 << 8 )  //!< 16进制显示
#define DBG_DMP_DAT_DEC     ( 1 << 9 )  //!< 10进制显示
#define DBG_DMP_DAT_CHAR    ( 1 << 10 ) //!< 字符显示
#define DBG_DMP_TAG_LINE    ( 1 << 16 ) //!< 显示行号
#define DBG_DMP_TAG_ADDR    ( 1 << 17 ) //!< 显示地址
#define DBG_DMP_TAG_LABEL   ( 1 << 18 ) //!< 显示标签
#define DBG_DMP_SEG_OFS     ( 24 )      //!< 分割标记偏移
#define DBG_DMP_SEG_NO      ( 0 << 24 ) //!< 无格式纯数据导出
#define DBG_DMP_SEG_8       ( 1 << 24 ) //!< 8字节分割
#define DBG_DMP_SEG_16      ( 1 << 25 ) //!< 16字节分割
#define DBG_DMP_SEG_32      ( 1 << 26 ) //!< 32字节分割
#define DBG_DMP_SEG_64      ( 1 << 27 ) //!< 64字节分割

#if ((DBG_DUMP_EN == 1) && (DS_DEBUG_MAIN == 1))
/**
 * \brief       数据格式化导出
 * \param       buf         数据缓存
 * \param       len         数据长度
 * \param       addr        mode == DBG_DMP_ADDR:   数据地址
 *                          mode == DBG_DMP_LINE:   行号
 * \param       mode        参考DBG_DMP_*定义
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_dump(char * buf, unsigned int len, void * addr, int mode);

/**
 * \brief       带标签的数据格式化导出
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         数据缓存
 * \param       len         数据长度
 * \param       addr        mode == DBG_DMP_ADDR:   数据地址
 *                          mode == DBG_DMP_LINE:   行号
 * \param       mode        参考DBG_DMP_*定义
 * \param       label       标签字符串
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_dump_label(const char * file, const char * func, int line,
        char * buf, unsigned int len, void * addr, int mode, char * label);

#ifdef __cplusplus
}
#endif

/** \brief      16进制导出, 带行号, 每行16字节 */
#define dbg_dmp_H(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, 0, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX, NULL); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带行号, 每行16字节 */
#define dbg_dmp_HC(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, 0, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR, NULL);\
    } \
}
/** \brief      16进制导出, 同时输出字符, 带地址, 每行16字节 */
#define dbg_dmp_HCA(__debug_switch__, buf, len, addr) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, addr, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_ADDR | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR, NULL);\
    } \
}
/** \brief      16进制导出, 带标签和行号, 每行16字节 */
#define dbg_dmp_HL(__debug_switch__, buf, len, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, 0, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX\
                | DBG_DMP_TAG_LABEL, label); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带标签和行号, 每行16字节 */
#define dbg_dmp_HCL(__debug_switch__, buf, len, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, 0, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR | DBG_DMP_TAG_LABEL,\
                label); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带标签和地址, 每行16字节 */
#define dbg_dmp_HCAL(__debug_switch__, buf, len, addr, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, addr, \
                __debug_switch__\
                | DBG_DMP_SEG_16 | DBG_DMP_TAG_ADDR | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR | DBG_DMP_TAG_LABEL,\
                label); \
    } \
}
/** \brief      纯字符导出, 无任何格式(可用于将内存或Flash数据导出到文件) */
#define dbg_dmp_C(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__FILE__, __func__, __LINE__, buf, len, 0, 0, NULL); \
    } \
}
#else
#define dbg_dump(...)
#define dbg_dump_label(...)

#define dbg_dmp_H(...)
#define dbg_dmp_HC(...)
#define dbg_dmp_HCA(...)
#define dbg_dmp_HL(...)
#define dbg_dmp_HCL(...)
#define dbg_dmp_HCAL(...)
#define dbg_dmp_C(...)
#endif /* ((DBG_DUMP_EN == 1) && (DS_DEBUG_MAIN == 1)) */
/** @} */
/**
 * \block:      TEST
 * @{ */
#ifdef __cplusplus
extern "C" {
#endif
/** \brief 测试列表结构体 */
typedef struct {
    char * info;                        //!< 列表项信息
    void * param;                       //!< 测试函数参数
    int (*func)(void *);                //!< 测试函数
}DBG_TESTLIST_T;

#if ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1))
/**
 * \brief       测试列表控制
 * \param       list            列表数组
 * \return      0: Success      <0: DBG_RET_t
 */
int dbg_testlist(DBG_TESTLIST_T * list, int size);
#ifdef __cplusplus
}
#endif

/** \brief      设置测试函数列表并加载 */
#define dbg_test_setlist(...) { \
    DBG_TESTLIST_T list[] = { __VA_ARGS__ }; \
    dbg_testlist(list, (sizeof(list) / sizeof(DBG_TESTLIST_T))); \
}
#else
#define dbg_testlist(...)

#define dbg_test_setlist(...)
#endif /* ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1)) */
/** @} */

/**
 * \block:      Code is far away from bug with the animal protecting
 * @{ */
/** normal {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃ ┳━┛     ┗━┳   ┃
 *   ┃               ┃
 *   ┃     ┻         ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** happy {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃  >       <    ┃
 *   ┃               ┃
 *   ///   v   ///   ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** cool {
 *     ┏━┓       ┏━┓  + +
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓  + +
 *   ┃               ┃
 *   ┃      ━        ┃  + + + + +
 *   #####--#####    ┃  +
 *   ┃               ┃  +
 *   ┃     ┻         ┃
 *   ┃               ┃  + +
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃  + + + +
 *       ┃       ┃  + 神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓  + +
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛  + + + +
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛  + + + +
}*/
/** @} */

#endif /* __DEBUG_MODULE_H__ */

