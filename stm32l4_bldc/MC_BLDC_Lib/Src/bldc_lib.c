/* Includes ------------------------------------------------------------------*/
#include "bldc_lib.h"
#include "tim.h"
#include <string.h>
#include "cmsis_os.h"
#include "main.h"

/* Data struct ---------------------------------------------------------------*/
stSIXSTEP_Base SIXSTEP_parameters;            /*!< Main SixStep structure*/ 


/* Variables -----------------------------------------------------------------*/

/* 6Step Not-Exported functions ----------------------------------------------*/
uint16_t 	MC_SixStep_PI_Controller(stSIXSTEP_PI_Param *, uint16_t);
void 			MC_SixStep_GetParameters(void);
uint32_t	MC_SixStep_GetElSpeedHz(void);
uint32_t 	MC_SixStep_GetMechSpeedRPM(void);
void		 	MC_SixStep_NextStep(void);
void 			MC_SixStep_Table(uint8_t);
void 			MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *);
void			MC_SixStep_Init(void);
void			MC_SixStep_Reset(void);
uint8_t   MC_SixStep_GetCurrentPosition(void);
uint8_t   MC_SixStep_GetDriverFault(void);
void 			MC_SixStep_ClearDriverFault(void);
void 			MC_SixStep_ShutDown(void);

/*
	Обработка ошибок
*/

void 			MC_SixStep_ClearErrors(void);
void 			MC_SixStep_SetErrorFlag(enSIXSTEP_Error err);


static uint8_t hallSensorPulse=FALSE;
static uint16_t timerOneSecCnt=0;


/*
MC_SixStep_TABLE    MC_SixStep_TABLE
*/

void MC_SixStep_Table(uint8_t step_number)
{ 
	 TIM_OC_InitTypeDef commConfigOC;
	 commConfigOC.Pulse = SIXSTEP_parameters.PWM_Value;//htim1.Instance->CCR1;
	 commConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	 commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	 commConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	 commConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	 commConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	 switch (step_number)
		{ 
			case 1:
			{                  
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
				  commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);				

					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2); 
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
					
    			commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);				
			}
		  break;
			
			case 2:
			{                                        
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
				  commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);	
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
				  commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
			

					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3); 
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);	
			}
			break; 
			
			case 3:  
			{    
					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);		
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1); 
								
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);				
				
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);	        
			}
			break; 
			
			case 4:
			{  
					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);	
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1); 

					
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);					
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);	

					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
				  commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);	
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
			}
			break;  
			
			case 5:  
			{                         
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);				

					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);					
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2); 
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);	
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3); 
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3); 				
			}
			break;
			
			case 6:
			{  
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
				  commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);	
				
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);  
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);				
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
					commConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3); 
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3); 
			}
			break; 			
		} 
}


void MC_SixStep_NextStep(void)
{
		MC_SixStep_GetMechSpeedRPM();  
		
		int8_t nextStep;
		
//		 if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
//		 { 
//				nextStep = (int8_t)SIXSTEP_parameters.positionStep + 1;
//						
//				if(nextStep>6)
//				{ 
//						nextStep = 1;
//				}
//		 }
//		 else
//		 {
//				nextStep = (int8_t)SIXSTEP_parameters.positionStep - 1;  
//				
//				if(nextStep < 1)
//				{ 
//						nextStep = 6; 
//				} 
//		 }
		 
		 nextStep = (int8_t)SIXSTEP_parameters.positionStep + 1;
						
		 if(nextStep>6)
		 { 
				nextStep = 1;
		 }
		
			/*
		 Сместим фазу для реверса
		 */
		 if(SIXSTEP_parameters.direction == SIXSTEP_DIR_BACKWARD)
		 {
				//nextStep += 1;
				nextStep += 5;
				if(nextStep>6)
				{
						nextStep -= 6;
				}
		 }
		 
		MC_SixStep_Table(nextStep);
}



void MC_SixStep_Reset(void) //Останов и сброс 
{   
		TIM_OC_InitTypeDef commConfigOC;

		commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
		commConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1); 
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1); 
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2); 
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2); 
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3); 
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3); 		
	
		SIXSTEP_parameters.speedFdbk 	= 0;
//		SIXSTEP_parameters.error 			= SIXSTEP_ERR_OK;
		SIXSTEP_parameters.PWM_Value	= 0;
//		SIXSTEP_parameters.direction  = SIXSTEP_DIR_FORWARD;
		SIXSTEP_parameters.status = SIXSTEP_STATUS_STOP;
	
		MC_SixStep_Set_PI_Param(&SIXSTEP_parameters.PI_Param); 
}

/*
******************** PI regulator***************************
*/

void MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *PI_Param)
{
		PI_Param->ReferenceSpeed = 0;   
		PI_Param->integralTermSum = 0;
			
		PI_Param->Kp_Gain = KP_GAIN;   
		PI_Param->Ki_Gain = KI_GAIN;      

		PI_Param->Lower_Limit_Output = LOWER_OUT_LIMIT;       
		PI_Param->Upper_Limit_Output = UPPER_OUT_LIMIT;     

		PI_Param->Max_PID_Output =  FALSE; 
		PI_Param->Min_PID_Output =  FALSE;
}


uint16_t MC_PI_Controller(stSIXSTEP_PI_Param *PI_Param, uint16_t speedFdbk)
{
		int32_t wProportional_Term=0, wIntegral_Term=0, wOutput_32=0,wIntegral_sum_temp=0;
		int32_t Error =0;
			
		Error = (PI_Param->ReferenceSpeed - speedFdbk);
		
		/* Proportional term computation*/
		wProportional_Term = PI_Param->Kp_Gain * Error;
			
		/* Integral term computation */
		if (PI_Param->Ki_Gain == 0)
		{
				SIXSTEP_parameters.PI_Param.integralTermSum = 0;
		}
		else
		{ 
				wIntegral_Term = PI_Param->Ki_Gain * Error;
				wIntegral_sum_temp = SIXSTEP_parameters.PI_Param.integralTermSum + wIntegral_Term;
				SIXSTEP_parameters.PI_Param.integralTermSum = wIntegral_sum_temp;
		}
  

    if(SIXSTEP_parameters.PI_Param.integralTermSum> KI_DIV * PI_Param->Upper_Limit_Output)
		{
       SIXSTEP_parameters.PI_Param.integralTermSum = KI_DIV* PI_Param->Upper_Limit_Output;
		}
   
    if(SIXSTEP_parameters.PI_Param.integralTermSum<-KI_DIV* PI_Param->Upper_Limit_Output)
		{
       SIXSTEP_parameters.PI_Param.integralTermSum = -KI_DIV* PI_Param->Upper_Limit_Output;
		}
    

               
    wOutput_32 = (wProportional_Term/KP_DIV) + (SIXSTEP_parameters.PI_Param.integralTermSum/KI_DIV);

  

	 if (wOutput_32 > PI_Param->Upper_Limit_Output)
	 {
				wOutput_32 = PI_Param->Upper_Limit_Output;		  			 	
	 }
	 else if (wOutput_32 < PI_Param->Lower_Limit_Output)
	 {
				wOutput_32 = PI_Param->Lower_Limit_Output;
	 }


		return((uint16_t)(wOutput_32)); 	
}


void MC_SixStep_SetSpeed(uint16_t speed_value)//Установка целевой скорости вращения
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

void MC_SixStep_StartMotor(enSIXSTEP_Direction dir)
{ 
		if(SIXSTEP_parameters.status == SIXSTEP_STATUS_STOP)
		{
			MC_SixStep_ClearDriverFault();
			MC_SixStep_ClearErrors();
			SIXSTEP_parameters.direction = dir;
			SIXSTEP_parameters.status = SIXSTEP_STATUS_INIT;
		}
}


void MC_SixStep_StopMotor(void)
{     
		SIXSTEP_parameters.status = SIXSTEP_STATUS_BREAK;
}

#define RAMP_STEPS_NUM		10
#define RAMP_PWM_STEP			0x5
void MC_SixStep_StartRamp(void)
{
		uint8_t rampSteps = 0;
		SIXSTEP_parameters.PWM_Value = 0;
		for(rampSteps = 0; rampSteps < RAMP_STEPS_NUM; rampSteps++)
		{
				SIXSTEP_parameters.PWM_Value += RAMP_PWM_STEP;
				vTaskDelay(1);
		}
}

void MC_SixStep_StopRamp(void)
{
	
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

const uint8_t hallPosTable[8] = {0, 1, 3, 2, 5, 6, 4, 0};//Перекодировка датчиков Холла в шаг

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
	
	
		stepPos = hallPosTable[hallPos];
		
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
		 if(SIXSTEP_parameters.status==SIXSTEP_STATUS_RAMP)
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

uint16_t MC_SixStep_GetGurrent(void) 
{
		SIXSTEP_parameters.currentFdbk=500;//реализовать
			
		return SIXSTEP_parameters.currentFdbk;
}


uint16_t MC_SixStep_GetVoltage(void)
{
		SIXSTEP_parameters.voltageFdbk=100;//реализовать
		return SIXSTEP_parameters.voltageFdbk;
}


void MC_SixStep_Init(void)
{
		HAL_TIMEx_ConfigCommutationEvent_IT(&PHASE_TIM, PHASE_TIM_TRIGGER_INPUT, TIM_COMMUTATION_SOFTWARE);
		HAL_TIMEx_HallSensor_Start_IT(&HALL_TIM);

    MC_SixStep_Reset();
}

/*
	Получаем параметры и сравниваем их с корректными
*/
void MC_SixStep_GetParameters(void)
{
		MC_SixStep_GetGurrent();
		if(!IS_BLDC_CURRENT(SIXSTEP_parameters.currentFdbk))
		{
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERCURRENT);
		}
		
		MC_SixStep_GetVoltage();		
		if(!IS_BLDC_VOLTAGE(SIXSTEP_parameters.voltageFdbk))
		{
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERVOLTAGE);
		}
		
		MC_SixStep_GetMechSpeedRPM();
		if(!IS_BLDC_RPM(SIXSTEP_parameters.speedFdbk))
		{
				MC_SixStep_SetErrorFlag(SIXSTEP_ERR_SPEEDFBKERROR);
		}
}


/*
*****************FSM контроллера двигателя*******************
*/

void 			MC_SixStep_Handler(void)
{
	
//	MC_SixStep_GetParameters();
//	
	if(SIXSTEP_parameters.error!=SIXSTEP_ERR_OK &&
		((SIXSTEP_parameters.status !=SIXSTEP_STATUS_FAULT) && 
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_BREAK) &&
		 (SIXSTEP_parameters.status !=SIXSTEP_STATUS_STOP))
	  )
	{
			SIXSTEP_parameters.status = SIXSTEP_STATUS_FAULT;
	}
	
//	if(MC_SixStep_GetDriverFault() == TRUE)
//	{
//			SIXSTEP_parameters.status = SIXSTEP_STATUS_FAULT;
//			MC_SixStep_SetErrorFlag(SIXSTEP_ERR_OVERCURRENT);
//	}
//	
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
							MC_SixStep_Table(SIXSTEP_parameters.positionStep);
//							MC_SixStep_NextStep();
							SIXSTEP_parameters.status=SIXSTEP_STATUS_RAMP;
							htim1.Instance->EGR|=TIM_EGR_COMG; //генерим событие коммутации
							
					}	
			}
			break;	

			
			case SIXSTEP_STATUS_RAMP://Плавное увеличение тока двигателя при старте
			{					
					MC_SixStep_StartRamp();
					SIXSTEP_parameters.status=SIXSTEP_STATUS_RUN;
				
					/*
						Обработать ошибку запуска
					*/
			}
			break;
			
						
			case SIXSTEP_STATUS_RUN:
			{
					//SIXSTEP_parameters.PWM_Value=5;//MC_PI_Controller(&PI_Parameters, SIXSTEP_parameters.speedFdbk);
//					if(SIXSTEP_parameters.flagIsSpeedNotZero == FALSE)//неожиданная остановка двигателя
//					{
//							SIXSTEP_parameters.status=SIXSTEP_STATUS_FAULT;
//							MC_SixStep_SetErrorFlag(SIXSTEP_ERR_UNEXPECTED_STOP);
//					}
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
				//error message
					
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
	
	/*
		Определим нулевую частоту оборотов двигателя
	*/
	
	timerOneSecCnt++;
	if(timerOneSecCnt>=100)
	{
			timerOneSecCnt=0;
			if(hallSensorPulse == FALSE)
			{
					SIXSTEP_parameters.flagIsSpeedNotZero=FALSE;	
			}
			
			hallSensorPulse = FALSE;
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
		osDelay(100);
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
					htim1.Instance->EGR|=TIM_EGR_COMG; //commutation signal
			 }
		}
}