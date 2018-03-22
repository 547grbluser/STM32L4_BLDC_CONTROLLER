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
void 			MC_SixStep_ElSpeedHzToBuf(uint16_t elSpeed);
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
#define BLDC_EL_SPEED_BUF_SIZE		12//48//1//48
static uint8_t hallSensorPulse=FALSE;
static uint16_t ElSpeedBuf[BLDC_EL_SPEED_BUF_SIZE] = {0};//буфер усреднения RPM
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
				prevStep = (int8_t)SIXSTEP_parameters.positionStep - 1;//- 2;//- 1;//+ 2;	
	  }
		else
		{
				prevStep = (int8_t)SIXSTEP_parameters.positionStep + 1;//+ 2;//+1//+ 4;	
		}
	 
	 
		if(prevStep > 6)
		{ 
			prevStep = prevStep - 6;
		}
		
		if(prevStep < 1)
		{ 
			prevStep = prevStep + 6;
		}
		
		MC_SixStep_Table(prevStep);
}

void 			MC_SixStep_ChargeCap(uint16_t time)
{
		MC_SixStep_ShutDown();
	
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_LOW);

		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
		
		MC_SixStep_ClearDriverFault();
	
		vTaskDelay(time);
	
		MC_SixStep_ShutDown();
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);	

		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
		MC_SixStep_ClearDriverFault();
}

void 		MC_SixStep_Reset(void) //Останов и сброс 
{   
		HAL_TIMEx_HallSensor_Stop_IT(&HALL_TIM); 
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);								
		MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_INACTIVE, TIM_OCNPOLARITY_HIGH);		

		htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
	
		SIXSTEP_parameters.speedFdbk 	= 0;
		SIXSTEP_parameters.PWM_Value	= 0;
		SIXSTEP_parameters.status = SIXSTEP_STATUS_STOP;
		SIXSTEP_parameters.prevStep = FALSE;
//		SIXSTEP_parameters.mode	=	SIXSTEP_MODE_MOTOR;
	
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
			SIXSTEP_parameters.faultCnt = 0;
			SIXSTEP_parameters.phaseCounter = 0;
			SIXSTEP_parameters.mode	=	SIXSTEP_MODE_MOTOR;
			MC_SixStep_Set_PI_Param(&SIXSTEP_parameters.PI_Param); 
			MC_SixStep_SetSpeed(MC_TARGET_SPEED);
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

/*
	Включить ток через кабель
*/
void 			MC_SixStep_OpenVTSwitch(void)
{
		if(SIXSTEP_parameters.status == SIXSTEP_STATUS_STOP)
		{
				HAL_TIMEx_HallSensor_Stop_IT(&HALL_TIM); 

				MC_SixStep_ClearDriverFault();
				MC_SixStep_ClearErrors();
				SIXSTEP_parameters.mode	=	SIXSTEP_MODE_OPEN_SWITCH;
				MC_SixStep_ChargeCap(BLDC_CHARGE_CAP_TIME);
			
				MC_SixStep_SetPhaseParam(TIM_CHANNEL_1, TIM_OCMODE_ACTIVE, TIM_OCNPOLARITY_HIGH);
				MC_SixStep_SetPhaseParam(TIM_CHANNEL_2, TIM_OCMODE_ACTIVE, TIM_OCNPOLARITY_HIGH);								
				MC_SixStep_SetPhaseParam(TIM_CHANNEL_3, TIM_OCMODE_ACTIVE, TIM_OCNPOLARITY_HIGH);		
				
				SIXSTEP_parameters.status = SIXSTEP_OPEN_VT_SWITCH;
				htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
		}	
}

/*
	Выключить ток через кабель
*/
void 			MC_SixStep_CloseVTSwitch(void)
{
		MC_SixStep_Reset();
}
/***********************************************************************/
void MC_SixStep_ElSpeedHzToBuf(uint16_t elSpeed)
{
		static uint8_t bufCnt = 0;
		
		ElSpeedBuf[bufCnt] = elSpeed;
		bufCnt++;
		
		if(bufCnt > BLDC_EL_SPEED_BUF_SIZE)
		{
				bufCnt = 0;
		}
}

uint32_t MC_SixStep_GetElSpeedHz(void) //Частота импульсов с датчиков Холла
{   

	//код определения частоты вращения двигателя
	
	 uint32_t freq = 0;
	 uint8_t  bufCnt;
	
	 if(SIXSTEP_parameters.flagIsSpeedNotZero==FALSE)
	 {
			return 0;
	 }
	 
	 for(bufCnt = 0; bufCnt < BLDC_EL_SPEED_BUF_SIZE; bufCnt++)
	 {
			freq += ElSpeedBuf[bufCnt];
	 }
	
	 if(freq==0)
	 {
			return 0;
	 }
	 else
	 {
			freq= HALL_TIM_FREQ * BLDC_EL_SPEED_BUF_SIZE/freq;
	 }
	 
	 return freq;
}


uint32_t MC_SixStep_GetMechSpeedRPM(void) //Частота вращения ротора двигателя в RPM
{  	
		SIXSTEP_parameters.speedFdbk=(uint16_t)(MC_SixStep_GetElSpeedHz() *  60 / NUM_POLE_PAIRS/PAIR/BLDC_SIX_STEP);
	
		if(SIXSTEP_parameters.speedFdbk > MC_TARGET_SPEED)
		{
				TEST_PIN_ON;
		}
		else
		{
				TEST_PIN_OFF;
		}
		
	
		return SIXSTEP_parameters.speedFdbk;
}




const uint8_t hallPosTable_FWD[8] = {0, 4, 6, 5, 2, 3, 1, 0};//Перекодировка датчиков Холла в шаг FW ok
const uint8_t hallPosTable_BWD[8] = {0, 1, 3, 2, 5, 6, 4, 0};//Перекодировка датчиков Холла в шаг BW

//const uint8_t hallPosTable_FWD[8] = {0, 5, 1, 6, 3, 4, 2, 0};//Перекодировка датчиков Холла в шаг FW bad
//const uint8_t hallPosTable_BWD[8] = {0, 2, 4, 3, 6, 1, 5, 0};//Перекодировка датчиков Холла в шаг BW

//const uint8_t hallPosTable_FWD[8] = {0, 6, 2, 1, 4, 5, 3, 0};//Перекодировка датчиков Холла в шаг FW bad
//const uint8_t hallPosTable_BWD[8] = {0, 3, 5, 4, 1, 2, 6, 0};//Перекодировка датчиков Холла в шаг BW

//const uint8_t hallPosTable_FWD[8] = {0, 1, 3, 2, 5, 6, 4, 0};//Перекодировка датчиков Холла в шаг FW bad
//const uint8_t hallPosTable_BWD[8] = {0, 4, 6, 5, 2, 3, 1, 0};//Перекодировка датчиков Холла в шаг BW

//const uint8_t hallPosTable_FWD[8] = {0, 2, 4, 3, 6, 1, 5, 0};//Перекодировка датчиков Холла в шаг FW bad
//const uint8_t hallPosTable_BWD[8] = {0, 5, 1, 6, 3, 4, 2, 0};//Перекодировка датчиков Холла в шаг BW

//const uint8_t hallPosTable_FWD[8] =  {0, 3, 5, 4, 1, 2, 6, 0};//Перекодировка датчиков Холла в шаг FW bad
//const uint8_t hallPosTable_BWD[8] =  {0, 6, 2, 1, 4, 5, 3, 0}; //Перекодировка датчиков Холла в шаг BW

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
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_HALL_SENSOR_ERROR);
				return 0;
		}
		
		return stepPos;
}

#define SIXSTEP_POS_CNT_ERR		5
void	MC_SixStep_HallFdbkVerify(void)
{
		
     static uint8_t stepPosPrev = 0xFF;
		 static uint8_t stepPosErrCnt	= 0;
	
		 uint8_t stepPosErrFlag = FALSE;
	
		 uint8_t stepPos = MC_SixStep_GetCurrentPosition();
	
		/*
			Проверим пропуски положения ротора
		*/
		 if((SIXSTEP_parameters.status==SIXSTEP_STATUS_INIT) || 
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_PREV_STEP)||
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_RAMP)	 ||
			 (SIXSTEP_parameters.status==SIXSTEP_STATUS_STOP))
		 {
				stepPosErrCnt = 0;
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
								stepPosErrFlag = TRUE;
						}
				}
				else
				{
						if(stepPosPrev!= 6)
						{
								stepPosErrFlag = TRUE;
						}
				}
		 }
		 else
		 {
				if(stepPos != 6)
				{
						if(stepPos != (stepPosPrev - 1))
						{
								stepPosErrFlag = TRUE;
						}
				}
				else
				{
						if(stepPosPrev!= 1)
						{
								stepPosErrFlag = TRUE;								
						}
				}		 
		 }
		 
		 
		 if(stepPosErrFlag)
		 {
				stepPosErrCnt++;
				stepPosPrev = 0xFF;
		 }
		 else
		 {
				stepPosErrCnt = 0;
		 }
		 
		 if(stepPosErrCnt < SIXSTEP_POS_CNT_ERR)
		 {
				stepPosPrev = stepPos;
		 }
		 else
		 {
				stepPosErrCnt = 0;
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_POSFBKERROR);
		 }
}


uint16_t MC_SixStep_GetCurrent(void) 
{
		uint32_t adcVoltage;
		static uint32_t adcVoltageCur0 = 0;
	
	/*
		При остановленном двигателе найдем смещение нуля
	*/
		if(SIXSTEP_parameters.status == SIXSTEP_STATUS_STOP)
		{
				adcVoltageCur0 = adcIntGetVoltage(ADC_MC_CURRENT);
		}
	
		adcVoltage = adcIntGetVoltage(ADC_MC_CURRENT);
		if(adcVoltage > adcVoltageCur0)
		{
			SIXSTEP_parameters.currentFdbk = (uint16_t)((adcVoltage - adcVoltageCur0) * MC_CURRENT_COEF);	
		}
		else
		{
				SIXSTEP_parameters.currentFdbk = 0;
		}
		return SIXSTEP_parameters.currentFdbk;
}


uint16_t MC_SixStep_GetVoltage(void)
{
		uint32_t adcVoltage;
		adcVoltage = adcIntGetVoltage(ADC_MC_VOLTAGE);		
		SIXSTEP_parameters.voltageFdbk = (uint16_t)(( adcVoltage* MC_VOLTAGE_COEF)/1000);
		return SIXSTEP_parameters.voltageFdbk;
}

uint32_t MC_SixStep_GetPhaseCounter(void)
{
	 return 	SIXSTEP_parameters.phaseCounter;
}

enSIXSTEP_SystStatus MC_SixStep_GetStatus(void)
{
		return SIXSTEP_parameters.status;
}

void MC_SixStep_Init(void)
{
		MC_SixStep_ShutDown();
		MC_SixStep_Reset();
		HAL_TIMEx_ConfigCommutationEvent_IT(&PHASE_TIM, PHASE_TIM_TRIGGER_INPUT, TIM_COMMUTATION_SOFTWARE);
		HAL_TIMEx_HallSensor_Stop_IT(&HALL_TIM);    
		MC_SixStep_ClearDriverFault();
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
	
	if(MC_SixStep_GetDriverFault() == TRUE)
	{
			MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERCURRENT);
	}

	if( SIXSTEP_parameters.error!=SIXSTEP_ERR_OK 					&&
		((SIXSTEP_parameters.status !=SIXSTEP_STATUS_FAULT) && 
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_BREAK) &&
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_STOP)	&&
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_RESTART))
	  )
	{
			SIXSTEP_parameters.status = SIXSTEP_STATUS_FAULT;
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
							SIXSTEP_parameters.PWM_Value = BLDC_PWM_START;		
							MC_SixStep_ChargeCap(BLDC_CHARGE_CAP_TIME);
							MC_SixStep_PrevStep();
							HAL_TIMEx_HallSensor_Stop_IT(&HALL_TIM); 
							
							htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации															
							SIXSTEP_parameters.status = SIXSTEP_STATUS_PREV_STEP;
							MC_SixStep_SetDelay(100);
					}	
			}
			break;	

			case SIXSTEP_STATUS_PREV_STEP:
			{			
					if(MC_SixStep_TimeoutDelay() && MC_SixStep_Ramp(BLDC_PWM_PREV_STEP_MAX, 1))
					{	
							SIXSTEP_parameters.PWM_Value = BLDC_PWM_START;	
							HAL_TIMEx_HallSensor_Start_IT(&HALL_TIM); 
							MC_SixStep_GetCurrentPosition();
							MC_SixStep_Table(SIXSTEP_parameters.positionStep);	
							htim1.Instance->EGR|=TIM_EGR_COMG; 
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RAMP;
							MC_SixStep_SetDelay(100);
					}					
			}
			break;
			
			case SIXSTEP_STATUS_RAMP://Плавное увеличение тока двигателя при старте
			{					
					if(MC_SixStep_Ramp(BLDC_PWM_RAMP_MAX, 1) && MC_SixStep_TimeoutDelay())
					{	
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RUN;
					}
			}
			break;
			
						
			case SIXSTEP_STATUS_RUN:
			{
					SIXSTEP_parameters.PWM_Value = MC_SixStep_PI_Controller(&SIXSTEP_parameters.PI_Param, SIXSTEP_parameters.speedFdbk);//48

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
					SIXSTEP_parameters.phaseCounter = 0;
			}
			break;
			
			case SIXSTEP_STATUS_FAULT:
			{
					SIXSTEP_parameters.faultCnt++;
					MC_SixStep_ShutDown();
					
				
					if((SIXSTEP_parameters.faultCnt < BLDC_FAULT_RESTART_N) 
							&& (SIXSTEP_parameters.mode	== SIXSTEP_MODE_MOTOR))
					{
							MC_SixStep_SetDelay(200);
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RESTART;
					}
					else
					{							
							SIXSTEP_parameters.status=SIXSTEP_STATUS_BREAK;						
					}
			}
			break;

			case SIXSTEP_STATUS_RESTART:
			{				
					if(MC_SixStep_TimeoutDelay())
					{
							SIXSTEP_parameters.status=SIXSTEP_STATUS_INIT;
							MC_SixStep_ClearDriverFault();
							MC_SixStep_ClearErrors();
					}
			}
			break;	

			case SIXSTEP_OPEN_VT_SWITCH:
			{	
					
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

/*
Наблюдается дребезг!
*/
uint8_t   MC_SixStep_GetDriverFault(void) //Overcurrent or undervoltage
{
		static GPIO_PinState pinStatePrev = GPIO_PIN_SET;
		uint8_t fault = FALSE;

		if(HAL_GPIO_ReadPin(FAULT_GPIO_Port, FAULT_Pin) == GPIO_PIN_RESET)
		{
				if(pinStatePrev == HAL_GPIO_ReadPin(FAULT_GPIO_Port, FAULT_Pin))
				{
						fault =  TRUE;
				}
		}
		
		pinStatePrev = HAL_GPIO_ReadPin(FAULT_GPIO_Port, FAULT_Pin);
				
		return fault;
}

void 			MC_SixStep_ClearDriverFault(void)//
{
		HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(F_CLR_GPIO_Port, F_CLR_Pin, GPIO_PIN_RESET);
		osDelay(50);
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
		if(SIXSTEP_parameters.mode	== SIXSTEP_MODE_MOTOR)
		{
			MC_SixStep_HallFdbkVerify();
			MC_SixStep_NextStep();
			SIXSTEP_parameters.flagIsSpeedNotZero = TRUE;
			hallSensorPulse = TRUE;	
			SIXSTEP_parameters.phaseCounter++;
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		if(htim->Instance == TIM2)
		{				
			 if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			 {
	//				if(SIXSTEP_parameters.prevStep == FALSE)
	//				{
							htim1.Instance->EGR|=TIM_EGR_COMG; //commutation signal
	//				}
//					if(SIXSTEP_parameters.positionStep == 6)
					{					
						MC_SixStep_ElSpeedHzToBuf(htim->Instance->CCR1);
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
