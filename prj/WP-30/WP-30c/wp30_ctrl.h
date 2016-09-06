/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_ctrl.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/11/2014 3:10:23 PM
 * Description        : 
 *******************************************************************************/
#ifndef __wp30_CTRL_H__
#define __wp30_CTRL_H__ 

/*-----------------------------------------------------------------------------}
 *  Í·ÎÄ¼þ 
 *-----------------------------------------------------------------------------{*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "wp30_cfg.h"
#include "MacroDef.h"
#include "drv_inc.h"
#include "wp30_ctrl_cfg.h"
#include "wp30_ctrl_isr.h"
#include "app_common.h"
#include "../common/sys_zone/sys_inf_zone.h"
#include "../common/dl/local_dl_fac.h"
#include "../common/dl/local_dl_app.h"
#include "wp30_ctrl_menu.h"

#include "./sys/sys_api.h"
#include "./debug/wp30_ctrl_dbg.h"
#include "./sys/wp30c_sys_variate.h"
#include "./sys/misc.h"
#include "./daemon/wp30_daemon.h"

#ifdef CFG_ICCARD
#include "./iccard/iccard.h"
#include "./iccard/iccard_hal.h"
#include "./iccard/iccard_func.h"
#include "./iccard/iccard_hal_gpio.h"
#include "./iccard/iccard_hw.h"
#endif

#ifdef CFG_MAGCARD
#include "./mag/magcard_hw.h"
#include "./mag/magcard.h"
#include "./mag/Magtek.h"
#include "./mag/IDTech.h"
#include "./mag/ENC_Magtek.h"
#include "./mag/Hcm4003.h"
#endif
#ifdef CFG_BLUETOOTH
#include "./bt/BlueTooth.h"
#endif

#ifdef CFG_RFID
#include "./rfid/Mifs_HDEFS.h"
#include "./rfid/mifs_sdefs.h"
#include "./rfid/mifscmd.h"
#include "./rfid/mifshal.h"
#include "./rfid/mifs.h"
#include "./rfid/mifs_paypass.h"
#endif
#include "./libdll/libdll.h"
#include "./test/fac_ctrl.h"
#ifdef CFG_FACTORY
//#include "./test/fac_ctrl.h"
#include "./test/factory.h"
#endif
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#include "./pinpad/pcikeystore.h"
#include "./pinpad/pinpad.h"
#include "./pinpad/padlib.h"
#include "./pinpad/pinpad_sys.h"
#include "./pinpad/pinpad_keystore.h"
#include "./pinpad/pinpad_parser.h"
#endif

#include "./pinpad/pinpad.h"
#include "./pinpad/pinpad_keystore.h"
#include "./pinpad/pinpad_parser.h"

#include "./lowpower/lowpower.h"
#ifdef CFG_FS
#include "./file/file.h"
#include "./file/filedef.h"
#endif

#include "./ctc_protocol/ctc_protocol.h"
#include "./ctc_protocol/ctc_sys.h"

#ifdef CFG_TPRINTER
#include "./tprinter/tp.h"
#include "./tprinter/tp_hal.h"
#include "./tprinter/tprinter.h"
#endif

#if defined(CFG_SECURITY_CHIP)
#include "./securitychip/drv_is8u256a.h"
#endif

#if defined(CFG_OPENSSL)
#include "./securitychip/sm.h"
#endif

#if defined(CFG_SCANER)
#include "./scaner/2D.h"
#endif 

#if defined(CFG_SE_MANAGE)
#include "./manage/appmanage.h"
#include "./manage/se_manage.h"
#endif

#endif
