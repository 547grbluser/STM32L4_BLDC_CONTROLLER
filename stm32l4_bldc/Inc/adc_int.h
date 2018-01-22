#ifndef ADC_INT_H
#define ADC_INT_H
#include "stm32l4xx_hal.h"
#include "calibr.h"

#define ADC_INT_CHANNEL_NUM				3
#define ADC_INT_CIRCLE_BUF_LEN		64 //количество элементов кольцевого буфера на один канал
#define ADC_INT_FILTER_LEN				56	

#define ADC_INT_VREF_VAL				1250//mV
#define ADC_INT_MAX_VAL					4095
#define ADC_INT_AVCC_VAL				3300//mV

#define VREFINT_CAL       ((uint16_t*) ((uint32_t)0x1FFF7A2A))  /* Internal temperature sensor, parameter VREFINT_CAL: Raw data acquired at a temperature of 30 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
                                                                /* This calibration parameter is intended to calculate the actual VDDA from Vrefint ADC measurement. */
typedef enum
{
		ADC_INT_CHANNEL_0=0,
		ADC_INT_CHANNEL_1=1,
		ADC_INT_CHANNEL_VREF=2,
}enADCChannels;


#define ADC_MC_VOLTAGE	ADC_INT_CHANNEL_0
#define ADC_MC_CURRENT	ADC_INT_CHANNEL_1

int 			adcIntInit(void);
uint16_t 	adcIntGet(enADCChannels channel);
uint16_t 	adcIntGetVoltage(enADCChannels channel);//mV
float 		adcIntGetCalibratedValue(enADCChannels channel, stCalibrValues calibr);

#endif
