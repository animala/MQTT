#include "fy_ntc.h"

#define NTC_R  10  //串联电阻

#if NTC_UP    //NTC在电源端
	#define NTC_ADC(ntc)   (u16)(((NTC_R*4095.0)/(ntc+NTC_R)))//阻值与ADC关系 NTC在上面  ADC值升序
#else   //NTC在接地端
	#define NTC_ADC(ntc)   (u16)(((ntc*4095.0)/(ntc+NTC_R)))//阻值与ADC关系 NTC在下面  ADC值降序
#endif
//温度阻值-ADC值表
//温度值上升排序
const unsigned int ntcData[]= {	
	NTC_ADC(	190.5562	),//-40
	NTC_ADC(	183.4132	),
	NTC_ADC(	175.6740	),
	NTC_ADC(	167.6467	),
	NTC_ADC(	159.5647	),
	NTC_ADC(	151.5975	),
	NTC_ADC(	143.8624	),
	NTC_ADC(	136.4361	),
	NTC_ADC(	129.3641	),
	NTC_ADC(	122.6678	),
	NTC_ADC(	116.3519	),
	NTC_ADC(	110.4098	),
	NTC_ADC(	104.8272	),
	NTC_ADC(	99.5847	),
	NTC_ADC(	94.6608	),
	NTC_ADC(	90.0326	),
	NTC_ADC(	85.6778	),
	NTC_ADC(	81.5747	),
	NTC_ADC(	77.7031	),
	NTC_ADC(	74.0442	),
	NTC_ADC(	70.5811	),
	NTC_ADC(	67.2987	),
	NTC_ADC(	64.1834	),
	NTC_ADC(	61.2233	),
	NTC_ADC(	58.4080	),
	NTC_ADC(	55.7284	),
	NTC_ADC(	53.1766	),
	NTC_ADC(	50.7456	),
	NTC_ADC(	48.4294	),
	NTC_ADC(	46.2224	),
	NTC_ADC(	44.1201	),
	NTC_ADC(	42.1180	),
	NTC_ADC(	40.2121	),
	NTC_ADC(	38.3988	),
	NTC_ADC(	36.6746	),
	NTC_ADC(	35.0362	),
	NTC_ADC(	33.4802	),
	NTC_ADC(	32.0035	),
	NTC_ADC(	30.6028	),
	NTC_ADC(	29.2750	),
	NTC_ADC(	28.0170	),
	NTC_ADC(	26.8255	),
	NTC_ADC(	25.6972	),
	NTC_ADC(	24.6290	),
	NTC_ADC(	23.6176	),
	NTC_ADC(	22.6597	),
	NTC_ADC(	21.7522	),
	NTC_ADC(	20.8916	),
	NTC_ADC(	20.0749	),
	NTC_ADC(	19.2988	),
	NTC_ADC(	18.5600	),
	NTC_ADC(	18.4818	),
	NTC_ADC(	18.1489	),
	NTC_ADC(	17.6316	),
	NTC_ADC(	16.9917	),
	NTC_ADC(	16.2797	),
	NTC_ADC(	15.5350	),
	NTC_ADC(	14.7867	),
	NTC_ADC(	14.0551	),
	NTC_ADC(	13.3536	),
	NTC_ADC(	12.6900	),
	NTC_ADC(	12.0684	),
	NTC_ADC(	11.4900	),
	NTC_ADC(	10.9539	),
	NTC_ADC(	10.4582	),
	NTC_ADC(	10.0000	),
	NTC_ADC(	9.5762	),
	NTC_ADC(	9.1835	),
	NTC_ADC(	8.8186	),
	NTC_ADC(	8.4784	),
	NTC_ADC(	8.1600	),
	NTC_ADC(	7.8608	),
	NTC_ADC(	7.5785	),
	NTC_ADC(	7.3109	),
	NTC_ADC(	7.0564	),
	NTC_ADC(	6.8133	),
	NTC_ADC(	6.5806	),
	NTC_ADC(	6.3570	),
	NTC_ADC(	6.1418	),
	NTC_ADC(	5.9343	),
	NTC_ADC(	5.7340	),
	NTC_ADC(	5.5405	),
	NTC_ADC(	5.3534	),
	NTC_ADC(	5.1725	),
	NTC_ADC(	4.9976	),
	NTC_ADC(	4.8286	),
	NTC_ADC(	4.6652	),
	NTC_ADC(	4.5073	),
	NTC_ADC(	4.3548	),
	NTC_ADC(	4.2075	),
	NTC_ADC(	4.0650	),
	NTC_ADC(	3.9271	),
	NTC_ADC(	3.7936	),
	NTC_ADC(	3.6639	),
	NTC_ADC(	3.5377	),
	NTC_ADC(	3.4146	),
	NTC_ADC(	3.2939	),
	NTC_ADC(	3.1752	),
	NTC_ADC(	3.0579	),
	NTC_ADC(	2.9414	),
	NTC_ADC(	2.8250	),
	NTC_ADC(	2.7762	),
	NTC_ADC(	2.7179	),
	NTC_ADC(	2.6523	),
	NTC_ADC(	2.5817	),
	NTC_ADC(	2.5076	),
	NTC_ADC(	2.4319	),
	NTC_ADC(	2.3557	),
	NTC_ADC(	2.2803	),
	NTC_ADC(	2.2065	),
	NTC_ADC(	2.1350	),
	NTC_ADC(	2.0661	),
	NTC_ADC(	2.0004	),
	NTC_ADC(	1.9378	),
	NTC_ADC(	1.8785	),
	NTC_ADC(	1.8225	),
	NTC_ADC(	1.7696	),
	NTC_ADC(	1.7197	),
	NTC_ADC(	1.6727	),
	NTC_ADC(	1.6282	),
	NTC_ADC(	1.5860	),
	NTC_ADC(	1.5458	),
	NTC_ADC(	1.5075	),
	NTC_ADC(	1.4707	),
	NTC_ADC(	1.4352	),
	NTC_ADC(	1.4006	),
	NTC_ADC(	1.3669	),
	NTC_ADC(	1.3337	),
	NTC_ADC(	1.3009	),
	NTC_ADC(	1.2684	),
	NTC_ADC(	1.2360	),
	NTC_ADC(	1.2037	),
	NTC_ADC(	1.1714	),
	NTC_ADC(	1.1390	),
	NTC_ADC(	1.1067	),
	NTC_ADC(	1.0744	),
	NTC_ADC(	1.0422	),
	NTC_ADC(	1.0104	),
	NTC_ADC(	0.9789	),
	NTC_ADC(	0.9481	),
	NTC_ADC(	0.9180	),
	NTC_ADC(	0.8889	),
	NTC_ADC(	0.8610	),
	NTC_ADC(	0.8346	),
	NTC_ADC(	0.8099	),
	NTC_ADC(	0.7870	),
	NTC_ADC(	0.7665	),
	NTC_ADC(	0.7485	),
	NTC_ADC(	0.7334	),
	NTC_ADC(	0.7214	),
	NTC_ADC(	0.7130	),//110
};

#if NTC_UP
//NTC 温度计算,输入当前ADC电压值 0.1du
//ADC值随温度上升而上升
s16 Ntc_GetTemp(u16 adc)
{
    u16 i;
    s16 tp;
    adc+=70;
    for (i=0; i<291; i++)
    {
        if (adc<ntcData[i])
        {
            break;
        }
    }
    if (i==0)
    {
        tp=-1000;//最低温度
    }
    else if (i==291)
    {
        tp=2500;//最高温度
    }
    else
    {
        tp=(adc-ntcData[i-1])*10/(ntcData[i]-ntcData[i-1])+(i-40-1)*10;
    }
    return tp;

}
#else
//NTC 温度计算,输入当前ADC电压值 0.1du
//ADC值随温度上升而下降
s16 Ntc_GetTemp(u16 adc)
{
    u16 i;
    s16 tp;
    adc+=70;
    for (i=0; i<151; i++)
    {
        if (adc>ntcData[i])
        {
            break;
        }
    }
    if (i==0)
    {
        tp=-1000;//最低温度
    }
    else if (i==291)
    {
        tp=1200;//最高温度
    }
    else
    {
        tp=(ntcData[i-1]-adc)*10/(ntcData[i-1]-ntcData[i])+(i-40-1)*10;
    }
    return tp;

}
#endif

