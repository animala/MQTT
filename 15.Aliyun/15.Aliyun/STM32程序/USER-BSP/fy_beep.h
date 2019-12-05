#ifndef __FY_BEEP_H
#define __FY_BEEP_H

#include "fy_includes.h"

/********ÆßÒô½×¶ÔÓ¦ÆµÂÊ*************/
typedef enum
{
    BEEP_TONE_DO_L = 262,
    BEEP_TONE_RE_L = 294,
    BEEP_TONE_MI_L = 330,
    BEEP_TONE_FA_L = 350,
    BEEP_TONE_SO_L = 392,
    BEEP_TONE_LA_L = 440,
    BEEP_TONE_SI_L = 523,
	
    BEEP_TONE_DO_M = 523,
    BEEP_TONE_RE_M = 587,
    BEEP_TONE_MI_M = 659,
    BEEP_TONE_FA_M = 698,
    BEEP_TONE_SO_M = 784,
    BEEP_TONE_LA_M = 880,
    BEEP_TONE_SI_M = 988
}_typdef_beep_tone;


void Beep_Configuration(u8 vol,u16 tone);
void Beep_SetVol(u8 vol);
void Beep_SetTone(u16 tone);
void Beep_PlayMusic(u8 vol,u16* musicscore);

#endif




/*********************************************END OF FILE********************************************/
