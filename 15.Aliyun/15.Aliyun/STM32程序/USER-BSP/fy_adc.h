#ifndef __FY_ADC_H
#define	__FY_ADC_H

#include "fy_includes.h"

#define ADC_FILTER 8

typedef enum {
    ADC_LIGHT = 0,
    ADC_NTC,
    ADC_VCC,
    ADC_CH_MAX
} _typdef_adc_align;

typedef struct _typdef_adc
{
    u16 buf[ADC_FILTER][ADC_CH_MAX];//DMA×ª»»ADCµÄ»º´æ
} _typdef_adc;


void Adc_Configuration(void);

#endif
/*********************************************END OF FILE********************************************/

