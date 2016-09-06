/**HEADER*********************************************************************
*
* Copyright (c) 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
*
******************************************************************************
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
******************************************************************************
*
* $FileName: soc_isr.c
* $Version :
* $Date    :
*
* Comments:
*
*  
*
*END*************************************************************************/
#include "types.h"
#include "soc.h"
//#include "derivative.h"
//#include "soc_pit.h"
//#include "soc_i2s.h"

typedef void (*int_isr_fptr_t)(void*);
static int_isr_fptr_t lowlevel_usb_isr_69 = NULL;
static void* isr_param_69 = NULL;


int32_t soc_install_isr( uint32_t vector, int_isr_fptr_t isr_ptr, void* isr_data)
{
    if(vector == 69)
    {
        lowlevel_usb_isr_69 = isr_ptr;
        isr_param_69 = isr_data;
    }
  
    return 1;
}

void USB_OTG_IRQHandler(void)
{
    if(lowlevel_usb_isr_69 != NULL)
    {
        lowlevel_usb_isr_69(isr_param_69);
    }
}


/* EOF */

