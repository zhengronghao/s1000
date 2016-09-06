#include "wp30_ctrl.h"


#ifdef CFG_FS
//#define lcd_cls()
//#define	lcd_display(x, y, mode, ...)    do {TRACE(__VA_ARGS__); TRACE("\r\n");} while(0)

/* extern uint32_t    DATA_SECTORS; */
void s_fsprompt_begformat(void)
{
	lcd_cls();
#if  defined(__DEBUG_Dx__)
	lcd_display(0, 0, DISP_CLRLINE | FONT_SIZE8, "FORMAT FS...");
#endif
}

void s_fsprompt_erase(uint32_t Addr)
{
#if  defined(__DEBUG_Dx__)
	int32_t   i, total;

    total = DATA_SECTORS + 2;
	lcd_display(0, 32, DISP_CLRLINE | FONT_SIZE8, "ERASE FS MEM...");
	if(Addr == FAT1_ADDR)
	{
        i = 1;
	}
	else if(Addr == FAT2_ADDR)
	{
        i = 2;
	}
	else
	{
		i = (Addr - DATA_ADDR)/DATA_SECTOR_SIZE + 3;
	}
	lcd_display(0, 40, DISP_CLRLINE | FONT_SIZE8, "%d%% OK", i*100/total);
#endif
}

void s_fsprompt_percent(uint16_t per)
{
#if  defined(__DEBUG_Dx__)
	static int32_t DispCnt;
	static const int8_t DispSSS[10]=">>>>>>>>";

	if(per == 0)
	{
		DispCnt = 5;
	}
	else
	{
		if(per > 100)
		{
			per = 100;
		}

		if(per == 100)
		{
			lcd_display(0, 56, DISP_CLRLINE | FONT_SIZE8, "PLEASE WAIT...");
		}
		else
		{
			lcd_display(0, 56, DISP_CLRLINE | FONT_SIZE8, "%d%% OK%s", per, DispSSS+DispCnt);
			if(--DispCnt == 0)
			{
				DispCnt = 5;
			}
		}
	}
#endif
}
#endif

