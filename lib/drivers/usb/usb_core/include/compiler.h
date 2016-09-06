/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: compiler.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file defines compiler related MACRO
*
*END************************************************************************/
#ifndef __compiler_h__
#define __compiler_h__ 1


#ifdef __cplusplus
extern "C" {
#endif
#if ((defined __CWCC__)||(defined __GNUC__))
#define PACKED_STRUCT_BEGIN
#define PACKED_STRUCT_END     __attribute__((__packed__))

#define PACKED_UNION_BEGIN
#define PACKED_UNION_END      __attribute__((__packed__))
#elif (defined __IAR_SYSTEMS_ICC__)
#define PACKED_STRUCT_BEGIN   __packed
#define PACKED_STRUCT_END
  
#define PACKED_UNION_BEGIN
#define PACKED_UNION_END      __packed

#elif (defined __CC_ARM)
#define PACKED_STRUCT_BEGIN   _Pragma("pack(1)")
#define PACKED_STRUCT_END    _Pragma("pack()")
  
#define PACKED_UNION_BEGIN    _Pragma("pack()")
#define PACKED_UNION_END    _Pragma("pack()")
#endif  
  

#ifdef __cplusplus
}
#endif

#endif
