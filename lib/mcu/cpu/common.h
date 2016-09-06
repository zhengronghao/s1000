/*
 * File:        common.h
 * Purpose:     File to be included by all project files
 *
 * Notes:
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/********************************************************************/
/* 
 * Include any toolchain specfic header files 
 */
#if (defined(CW))
  #include "cw.h"
#elif (defined(__MWERKS__))
  #include "mwerks.h"
#elif (defined(__DCC__))
  #include "build/wrs/diab.h"
#elif (defined(__ghs__))
  #include "build/ghs/ghs.h"
#elif (defined(__GNUC__))
  #include "build/gnu/gnu.h"
#elif (defined(IAR))
//  #include "iar.h"
#else
#warning "No toolchain specific header included"
#endif

/* 
 * Include common utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "startup.h"
#include "typedef.h"
#if (defined(IAR))
  #include "intrinsics.h"
#else
#endif
/* 
 * Include the platform specific header file 
 */
//-------------------------------
#include "hw_k21p121m50.h"


/********************************************************************/
#endif /* _COMMON_H_ */
