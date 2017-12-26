#include "adc_int.h"
#include "stm32l4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"


extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;


uint16_t ADC_value[ADC_INT_CIRCLE_BUF_LEN][ADC_INT_CHANNEL_NUM];

uint16_t adcIntGetFilteredValue(enADCChannels channel);


#define ADC_INT_TEST_TASK_STACK_SIZE	128
static void ADC_Int_Test_Task(void *pvParameters);

int adcIntInit(void)
{
		HAL_StatusTypeDef err=HAL_OK;
		err=HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_value,(ADC_INT_CHANNEL_NUM*ADC_INT_CIRCLE_BUF_LEN));
}

uint16_t adcIntGet(enADCChannels channel)
{
		return adcIntGetFilteredValue(channel);
}


uint16_t adcIntGetVoltage(enADCChannels channel)//mV
{
		uint16_t vrefValue=0;
		uint32_t adcValue=0;
		uint16_t result;
		uint16_t vrefCal= *VREFINT_CAL; // read VREFINT_CAL_ADDR memory location
	
		vrefValue=adcIntGetFilteredValue(ADC_INT_CHANNEL_VREF);
		if(!vrefValue)
		{
				return 0;
		}
		
		adcValue=adcIntGetFilteredValue(channel);
	
		uint16_t avcc=(ADC_INT_AVCC_VAL*vrefCal/vrefValue);
		result=(uint16_t)((adcValue*avcc)/ADC_INT_MAX_VAL);
	
		return result;	
}

uint16_t adcIntGetFilteredValue(enADCChannels channel)
{
		uint16_t 	bufCnt=0;
		int16_t 	bufIndex=0;
		uint32_t 	result=0;

		bufIndex=((ADC_INT_CHANNEL_NUM*ADC_INT_CIRCLE_BUF_LEN)-hdma_adc1.Instance->CNDTR)/ADC_INT_CHANNEL_NUM;

		bufCnt=ADC_INT_FILTER_LEN;

		while(bufCnt)
		{
				result+=ADC_value[bufIndex][channel];
				bufIndex--;
				
				if(bufIndex<0)
				{
						bufIndex=(ADC_INT_CIRCLE_BUF_LEN-1);
				}
						
				bufCnt--;
		}
		
		return (uint16_t)(result/ADC_INT_FILTER_LEN);
}

float adcIntGetCalibratedValue(enADCChannels channel, stCalibrValues calibr)
{
		float result=0.0;
		uint16_t voltage=0;
	
		voltage=adcIntGetVoltage(channel);	
	
		result=(((float)voltage-calibr.pointVoltage_1)*(calibr.pointValue_2-calibr.pointValue_1)/(calibr.pointVoltage_2-calibr.pointVoltage_1)) + calibr.pointValue_1;
		return result;
}
