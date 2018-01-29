/* Includes ------------------------------------------------------------------*/
#include "bldc_lib.h"
#include "tim.h"
#include <string.h>
#include "cmsis_os.h"
#include "main.h"
#include "adc_int.h"

/* Data struct ---------------------------------------------------------------*/
stSIXSTEP_Base SIXSTEP_parameters;            /*!< Main SixStep structure*/ 


/* Variables -----------------------------------------------------------------*/

/* 6Step Not-Exported functions ----------------------------------------------*/

void			MC_SixStep_Reset(void);

void 			MC_SixStep_GetParameters(void);
uint32_t	MC_SixStep_GetElSpeedHz(void);
void		 	MC_SixStep_DetectSpeedZero(void);

void 			MC_SixStep_SetPhaseParam(uint32_t Channel, uint32_t OCMode, uint32_t OCNPolarity);
void		 	MC_SixStep_NextStep(void);
void 			MC_SixStep_PrevStep(void);
uint8_t   MC_SixStep_GetCurrentPosition(void);
void 			MC_SixStep_Table(uint8_t);
uint8_t 	MC_SixStep_Ramp(uint8_t maxVal, uint8_t step);

void 			MC_SixStep_ChargeCap(uint16_t time);

/*
	IR2133
*/
uint8_t   MC_SixStep_GetDriverFault(void);
void 			MC_SixStep_ClearDriverFault(void);
void 			MC_SixStep_ShutDown(void);

/*
	Обработка ошибок
*/
void 			MC_SixStep_ClearErrors(void);
void 			MC_SixStep_SetErrorFlag(enSIXSTEP_Error err);

/*
	Задержки
*/
void 			MC_SixStep_SetDelay(uint32_t delay);
uint8_t 	MC_SixStep_TimeoutDelay(void);
/*
*********************Static variables*****************************
*/
static uint8_t hallSensorPulse=FALSE;
/*
*******************************************************************
*/

void MC_SixStep_SetPhaseParam(uint32_t Channel, uint32_t OCMode, uint32_t OCNPolarity)
{
		TIM_OC_InitTypeDef commConfigOC;
		commConfigOC.Pulse = SIXSTEP_parameters.PWM_Value;
		commConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
		commConfigOC.OCNPolarity = OCNPolarity;
		commConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		commConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		commConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
		commConfigOC.OCMode = OCMode;


		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, Channel);
		HAL_TIM_OC_Start(&htim1, Channel);
	
		if(OCMode == TIM_OCMODE_PWM1)
		{
				HAL_TIMEx_OCN_Stop(&htim1, Channel);
		}
		else
		{
				HAL_TIMEx_OCN_Start(&htim1, Channel);
		}
}

/*
MC_SixStep_TABLE    MC_SixStep_TABLE
*/

void MC_SixStep_Table(uint8_t step_number)
{ 	
	 switch (step_number)
		{ 
			case 1:
			{                  				
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);				
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
			}
		  break;
			
			case 2:
			{     
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);				
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);
			}
			break; 
			
			case 3:  
			{ 
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);					
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);
			}
			break; 
			
			case 4:
			{ 
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);				
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);					
			}
			break;  
			
			case 5:  
			{  
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);			
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);							
			}
			break;
			
			case 6:
			{ 
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);		
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);	
					MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
			}
			break; 			
		} 
}


void MC_SixStep_NextStep(void)
{		
		int8_t nextStep;
		 
//		nextStep = (int8_t)SIXSTEP_parameters.positionStep + 1;		 
//		if(nextStep>6)
//		{ 
//			nextStep = 1;
//		}
		
		if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
	  {			
				nextStep = (int8_t)SIXSTEP_parameters.positionStep + 1;	
	  }
		else
		{
				nextStep = (int8_t)SIXSTEP_parameters.positionStep - 1;	
		}
		
		if(nextStep>6)
		{ 
			nextStep = 1;
		}
		
		if(nextStep<1)
		{ 
			nextStep = 6;
		}		
				
	
		MC_SixStep_Table(nextStep);
}

void MC_SixStep_PrevStep(void)
{		
		int8_t prevStep;
		 
	  if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
	  {			
				prevStep = (int8_t)SIXSTEP_parameters.positionStep + 2;	
	  }
		else
		{
				prevStep = (int8_t)SIXSTEP_parameters.positionStep + 4;	
		}
	 
	 
		if(prevStep > 6)
		{ 
			prevStep = prevStep - 6;
		}
		
		MC_SixStep_Table(prevStep);
}

void 			MC_SixStep_ChargeCap(uint16_t time)
{
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);

//		SIXSTEP_parameters.PWM_Value	=  1;
//		
//		
////		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);
////		HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_1);
////		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
//		
//	  MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
//	
//		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);	
//		HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_2);
//		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
//	
//			MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);	
////	
////		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_PWM1, TIM_OCNPOLARITY_LOW);	
////		HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_3);
////		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);	
	
		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
	
		vTaskDelay(time);
	
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);	

		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
}

void 		MC_SixStep_Reset(void) //Останов и сброс 
{   	
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);		

		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
	
		SIXSTEP_parameters.speedFdbk 	= 0;
		SIXSTEP_parameters.PWM_Value	= 0;
		SIXSTEP_parameters.status = SIXSTEP_STATUS_STOP;
		SIXSTEP_parameters.prevStep = FALSE;
	
		MC_SixStep_Set_PI_Param(&SIXSTEP_parameters.PI_Param); 
}

/*
******************** PI regulator***************************
*/

void 		MC_SixStep_SetSpeed(uint16_t speed_value)//Установка целевой скорости вращения
{
   if(IS_BLDC_RPM(speed_value))
	 {
			SIXSTEP_parameters.PI_Param.ReferenceSpeed = speed_value;
	 } 
	 else
	 {
			//print error
	 }
}
/*
*******************************************************************
*/

void 		MC_SixStep_StartMotor(enSIXSTEP_Direction dir)
{ 
		if(SIXSTEP_parameters.status == SIXSTEP_STATUS_STOP)
		{
			MC_SixStep_ClearDriverFault();
			MC_SixStep_ClearErrors();
			SIXSTEP_parameters.direction = dir;
			SIXSTEP_parameters.status = SIXSTEP_STATUS_INIT;
		}
}


void 		MC_SixStep_StopMotor(void)
{     
		SIXSTEP_parameters.status = SIXSTEP_STATUS_BREAK;
}

/**********************************************************************/

uint8_t 	MC_SixStep_Ramp(uint8_t maxVal, uint8_t step)
{	
		if(SIXSTEP_parameters.PWM_Value  < maxVal)
		{
				SIXSTEP_parameters.PWM_Value += step;
				htim1.Instance->CCR1 = SIXSTEP_parameters.PWM_Value; 
				htim1.Instance->CCR2 = SIXSTEP_parameters.PWM_Value; 
				htim1.Instance->CCR3 = SIXSTEP_parameters.PWM_Value; 
				return FALSE;
		}
		else
		{
				return TRUE;
		}
}


uint32_t MC_SixStep_GetElSpeedHz(void) //Частота импульсов с датчиков Холла
{   

	//код определения частоты вращения двигателя
	
	 uint32_t freq;
	
	 if(SIXSTEP_parameters.flagIsSpeedNotZero==FALSE)
	 {
			return 0;
	 }
	
	 if(HALL_TIM.Instance->CCR1==0)
	 {
			return 0;
	 }
	 else
	 {
			freq= HALL_TIM_FREQ/HALL_TIM.Instance->CCR1;
	 }
	 
	 return freq;
}


uint32_t MC_SixStep_GetMechSpeedRPM(void) //Частота вращения ротора двигателя в RPM
{  	
		SIXSTEP_parameters.speedFdbk=(uint32_t)(MC_SixStep_GetElSpeedHz() *  60 / NUM_POLE_PAIRS);
		return SIXSTEP_parameters.speedFdbk;
}


//const uint8_t hallPosTable_FWD[8] = {0, 3, 5, 4, 1, 2, 6, 0};//Перекодировка датчиков Холла в шаг FW
//const uint8_t hallPosTable_BWD[8] = {0, 4, 6, 5, 2, 3, 1, 0};//Перекодировка датчиков Холла в шаг BW
const uint8_t hallPosTable_FWD[8] = {0, 3, 5, 4, 1, 2, 6, 0};//Перекодировка датчиков Холла в шаг FW
const uint8_t hallPosTable_BWD[8] = {0, 6, 2, 1, 4, 5, 3, 0};//Перекодировка датчиков Холла в шаг BW

uint8_t   MC_SixStep_GetCurrentPosition(void) //Текущее положение ротора
{
		uint8_t HALL_1=0;
		uint8_t HALL_2=0;
		uint8_t HALL_3=0;
		uint8_t hallPos = 0;
		uint8_t stepPos = 0;
	
		
	
		HALL_1=HAL_GPIO_ReadPin(HALL_1_GPIO_Port, HALL_1_Pin);
		HALL_2=HAL_GPIO_ReadPin(HALL_2_GPIO_Port, HALL_2_Pin);
		HALL_3=HAL_GPIO_ReadPin(HALL_3_GPIO_Port, HALL_3_Pin);
	
		hallPos=((HALL_1<<2)|(HALL_2<<1)|(HALL_3));
	
	 if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
	 {	
			stepPos = hallPosTable_FWD[hallPos];
	 }
	 else
	 {
			stepPos = hallPosTable_BWD[hallPos];
	 }
		
		if(stepPos)
		{
				SIXSTEP_parameters.positionStep=stepPos;
		}
		else //Недопустимый сигнал датчиков (все 0 или 1)
		{
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
				return 0;
		}
		
		return stepPos;
}

void	MC_SixStep_HallFdbkVerify(void)
{
		
     static uint8_t stepPosPrev = 0xFF;
	
		 uint8_t stepPos = MC_SixStep_GetCurrentPosition();
	
		/*
			Проверим пропуски положения ротора
		*/
		 if((SIXSTEP_parameters.status==SIXSTEP_STATUS_INIT) || 
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_PREV_STEP)||
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_RAMP)	 ||
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_STOP))
		 {
				stepPosPrev = 0xFF;
		 }
	
		 if(stepPosPrev == 0xFF)//пропустим проверку
		 {
				stepPosPrev = stepPos;
				return ;
		 }
	
		 if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
		 {	
				if(stepPos!= 1)
				{
						if(stepPos!= (stepPosPrev + 1))
						{
								MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
								stepPosPrev = 0xFF;
								return;
						}
				}
				else
				{
						if(stepPosPrev!= 6)
						{
								MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
								stepPosPrev = 0xFF;
								return;
						}
				}
		 }
		 else
		 {
				if(stepPos != 6)
				{
						if(stepPos != (stepPosPrev - 1))
						{
								MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
								stepPosPrev = 0xFF;
								return;
						}
				}
				else
				{
						if(stepPosPrev!= 1)
						{
								MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
								stepPosPrev = 0xFF;
								return;
						}
				}		 
		 }
		 
		 stepPosPrev = stepPos;
}

#define MC_CURRENT_COEF 10
uint16_t MC_SixStep_GetCurrent(void) 
{
		uint32_t adcVoltage;
		adcVoltage = adcIntGetVoltage(ADC_MC_CURRENT);
		SIXSTEP_parameters.currentFdbk = (uint16_t)(adcVoltage * MC_CURRENT_COEF);		
		return SIXSTEP_parameters.currentFdbk;
}

#define MC_VOLTAGE_COEF 100
uint16_t MC_SixStep_GetVoltage(void)
{
		uint32_t adcVoltage;
		adcVoltage = adcIntGetVoltage(ADC_MC_VOLTAGE);		
		SIXSTEP_parameters.voltageFdbk = (uint16_t)(( adcVoltage* MC_VOLTAGE_COEF)/1000);
		return SIXSTEP_parameters.voltageFdbk;
}

enSIXSTEP_SystStatus MC_SixStep_GetStatus(void)
{
		return SIXSTEP_parameters.status;
}

void MC_SixStep_Init(void)
{
		HAL_TIMEx_ConfigCommutationEvent_IT(&PHASE_TIM, PHASE_TIM_TRIGGER_INPUT, TIM_COMMUTATION_SOFTWARE);
		HAL_TIMEx_HallSensor_Start_IT(&HALL_TIM);
	
//		MC_SixStep_ChargeCap(1000);
    MC_SixStep_Reset();
}

/*
	Получаем параметры и сравниваем их с корректными
*/
void MC_SixStep_GetParameters(void)
{
		MC_SixStep_GetCurrent();
//		if(!IS_BLDC_CURRENT(SIXSTEP_parameters.currentFdbk))
//		{
//				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERCURRENT);
//		}
		
		MC_SixStep_GetVoltage();		
//		if(!IS_BLDC_VOLTAGE(SIXSTEP_parameters.voltageFdbk))
//		{
//				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERVOLTAGE);
//		}
		
		MC_SixStep_GetMechSpeedRPM();
//		if(!IS_BLDC_RPM(SIXSTEP_parameters.speedFdbk))
//		{
//				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_SPEEDFBKERROR);
//		}
}


void 	MC_SixStep_DetectSpeedZero(void)//Вызывается с частотой FSM (100 Hz)
{
	static uint16_t timerZeroSpeedCnt=0;
	
	timerZeroSpeedCnt++;
	if(timerZeroSpeedCnt>=10)
	{
			timerZeroSpeedCnt=0;
			if(hallSensorPulse == FALSE)
			{
					SIXSTEP_parameters.flagIsSpeedNotZero=FALSE;	
			}
			
			hallSensorPulse = FALSE;
	}	
}

/*
*****************FSM контроллера двигателя*******************
*/
void 			MC_SixStep_Handler(void)
{
	/*
		Определим нулевую частоту оборотов двигателя
	*/
	MC_SixStep_DetectSpeedZero();
	
	MC_SixStep_GetParameters();

	if( SIXSTEP_parameters.error!=SIXSTEP_ERR_OK 					&&
		((SIXSTEP_parameters.status !=SIXSTEP_STATUS_FAULT) && 
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_BREAK) &&
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_STOP))
	  )
	{
			SIXSTEP_parameters.status = SIXSTEP_STATUS_FAULT;
	}
	
	if(MC_SixStep_GetDriverFault() == TRUE)
	{
			MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERCURRENT);
	}
	
	switch(SIXSTEP_parameters.status)
	{		
			case SIXSTEP_STATUS_STOP: //
			{
					
			}
			break;	
			
			case SIXSTEP_STATUS_INIT: //Нахождение нач. положения ротора
			{						
					MC_SixStep_GetCurrentPosition();
					
				
					if(SIXSTEP_parameters.error!=SIXSTEP_ERR_OK)
					{							
							SIXSTEP_parameters.status=SIXSTEP_STATUS_FAULT;
					}
					else
					{
							SIXSTEP_parameters.PWM_Value = 0;		
							MC_SixStep_ChargeCap(100);
							//MC_SixStep_Table(SIXSTEP_parameters.positionStep);	
							MC_SixStep_PrevStep();
							SIXSTEP_parameters.prevStep = TRUE;
							
							htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации															
							SIXSTEP_parameters.status = SIXSTEP_STATUS_PREV_STEP;//SIXSTEP_STATUS_RAMP;	
							MC_SixStep_SetDelay(50);
					}	
			}
			break;	

			case SIXSTEP_STATUS_PREV_STEP:
			{			
					if(MC_SixStep_TimeoutDelay() && MC_SixStep_Ramp(60, 5))
					{	
							SIXSTEP_parameters.PWM_Value = 0;	
							SIXSTEP_parameters.prevStep = FALSE;
							MC_SixStep_GetCurrentPosition();
							MC_SixStep_Table(SIXSTEP_parameters.positionStep);	
							htim1.Instance->EGR|=TIM_EGR_COMG; 
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RAMP;
							MC_SixStep_SetDelay(200);
					}					
			}
			break;
			
			case SIXSTEP_STATUS_RAMP://Плавное увеличение тока двигателя при старте
			{					
					if(MC_SixStep_Ramp(60, 1) && MC_SixStep_TimeoutDelay())
					{	
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RUN;
					}
			}
			break;
			
						
			case SIXSTEP_STATUS_RUN:
			{
					//SIXSTEP_parameters.PWM_Value = MC_PI_Controller(&SIXSTEP_parameters.PI_Parameters, SIXSTEP_parameters.speedFdbk);

					
					if(SIXSTEP_parameters.flagIsSpeedNotZero == FALSE)//неожиданная остановка двигателя
					{
							SIXSTEP_parameters.status=SIXSTEP_STATUS_FAULT;
							MC_SixStep_SetErrorFlag(SIXSTEP_ERR_UNEXPECTED_STOP);
					}
			}
			break;
			
			case SIXSTEP_STATUS_BREAK:
			{								
					MC_SixStep_Reset();
					SIXSTEP_parameters.status=SIXSTEP_STATUS_STOP;
			}
			break;
			
			case SIXSTEP_STATUS_FAULT:
			{				
					MC_SixStep_ShutDown();
				  SIXSTEP_parameters.status=SIXSTEP_STATUS_BREAK;
			}
			break;			
			
			default:
			{
					SIXSTEP_parameters.status=SIXSTEP_STATUS_BREAK;
			}
			break;
	}
}

/*
**********************IR2133 signals***************************
*/
uint8_t   MC_SixStep_GetDriverFault(void) //Overcurrent or undervoltage
{
		return !HAL_GPIO_ReadPin(FAULT_GPIO_Port, FAULT_Pin);
}

void 			MC_SixStep_ClearDriverFault(void)//
{
		HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(F_CLR_GPIO_Port, F_CLR_Pin, GPIO_PIN_RESET);
		osDelay(500);
		HAL_GPIO_WritePin(F_CLR_GPIO_Port, F_CLR_Pin, GPIO_PIN_SET);		
}

void 			MC_SixStep_ShutDown(void)
{
		HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, GPIO_PIN_SET);
}
/*
****************************************************************  
*/
void 			MC_SixStep_ClearErrors(void)
{
		SIXSTEP_parameters.error=SIXSTEP_ERR_OK;
}

void 			MC_SixStep_SetErrorFlag(enSIXSTEP_Error err)
{
		SIXSTEP_parameters.error |= err;
}

enSIXSTEP_Error	MC_SixStep_GetErrorCode(void)
{
		return SIXSTEP_parameters.error;
}
/*
****************************************************************
*/

void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim) //
{
	if(htim->Instance == TIM1)
	{		
			MC_SixStep_HallFdbkVerify();
			MC_SixStep_NextStep();
			SIXSTEP_parameters.flagIsSpeedNotZero = TRUE;
			hallSensorPulse = TRUE;	
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		if(htim->Instance == TIM2)
		{				
			 if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			 {
					if(SIXSTEP_parameters.prevStep == FALSE)
					{
							htim1.Instance->EGR|=TIM_EGR_COMG; //commutation signal
					}
			 }
		}
}
/*
**********************Nonblocking delay************************
*/

static uint32_t mcDelay;
static uint32_t tickstart;
void 			MC_SixStep_SetDelay(uint32_t delay)
{
	mcDelay = delay;
	tickstart = HAL_GetTick();
}

uint8_t MC_SixStep_TimeoutDelay(void)
{
	  if(mcDelay != HAL_MAX_DELAY)
    {
      if((mcDelay == 0U) || ((HAL_GetTick()-tickstart) > mcDelay))
      {
					return TRUE;
			}
			else
			{
					return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
}
