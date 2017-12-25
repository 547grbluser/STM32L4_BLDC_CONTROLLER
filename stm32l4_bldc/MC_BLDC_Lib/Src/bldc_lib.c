/* Includes ------------------------------------------------------------------*/
#include "bldc_lib.h"
#include "tim.h"
#include <string.h>
#include "cmsis_os.h"

/* Data struct ---------------------------------------------------------------*/
stSIXSTEP_Base SIXSTEP_parameters;            /*!< Main SixStep structure*/ 
stSIXSTEP_PI_Param PI_Parameters;           /*!< SixStep PI regulator structure*/ 

/* Variables -----------------------------------------------------------------*/

/* 6Step Not-Exported functions ----------------------------------------------*/
int16_t 	MC_SixStep_PI_Controller(stSIXSTEP_PI_Param *, int16_t);
int32_t	 	MC_SixStep_GetElSpeedHz(void);
int32_t 	MC_SixStep_GetMechSpeedRPM(void);
void		 	MC_SixStep_NextStep(void);
void 			MC_SixStep_Table(uint8_t);
void 			MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *);
void 			MC_SixStep_InitMainData(void);
void			MC_SixStep_Init(void);
void			MC_SixStep_Reset(void);
uint8_t   MC_SixStep_GetCurrentPosition(void);


/** @defgroup MC_SixStep_TABLE    MC_SixStep_TABLE
  *  @{
    * @brief Set the peripherals (TIMx, GPIO etc.) for each step
    * @param  step_number: step number selected
    * @retval None
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
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);    

					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
					
    			commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
			}
		  break;
			
			case 2:
			{                                        
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);

					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3); 
			}
			break; 
			
			case 3:  
			{    
					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);			
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);  
				
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
				
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);        
			}
			break; 
			
			case 4:
			{  
					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);			
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1); 
					
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);					
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);    

					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);				
			}
			break;  
			
			case 5:  
			{                         
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);

					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);					
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);   
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);        
			}
			break;
			
			case 6:
			{  
					commConfigOC.OCMode = TIM_OCMODE_PWM1;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1);
					HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
				
					commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
				
					commConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
					HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
					HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);  
			}
			break; 			
		} 
}

/**
  * @}
  */

/** @defgroup MC_SixStep_NEXT_step    MC_SixStep_NEXT_step
  *  @{
    * @brief Generate the next step number according with the direction (CW or CCW)
    * @retval uint8_t SIXSTEP_parameters.status
  */
void MC_SixStep_NextStep(void)
{
		 MC_SixStep_GetMechSpeedRPM();    
		
		 if(PI_Parameters.Reference>=0)
		 { 
				SIXSTEP_parameters.positionStep++;
						
				if(SIXSTEP_parameters.positionStep>6)
				{ 
						SIXSTEP_parameters.positionStep = 1;
				}
		 }
		 else
		 {
				SIXSTEP_parameters.positionStep--;  
				
				if(SIXSTEP_parameters.positionStep < 1)
				{ 
						SIXSTEP_parameters.positionStep = 6; 
				} 
		 }
		 
		MC_SixStep_Table(SIXSTEP_parameters.positionStep);
}



/** @defgroup MC_SixStep_RESET    MC_SixStep_RESET
  *  @{
    * @brief Reset all variables used for 6Step control algorithm
    * @retval None
  */

void MC_SixStep_Reset(void)
{   
	 MC_SixStep_Set_PI_Param(&PI_Parameters); 
}



void MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *PI_Param)
{
		if(SIXSTEP_parameters.direction == SIXSTEP_DIR_FORWARD)
		{
				PI_Param->Reference = SIXSTEP_parameters.speedTarget;   
		}
		else
		{
				PI_Param->Reference = -SIXSTEP_parameters.speedTarget;  
		}		
		
		PI_Param->Kp_Gain = SIXSTEP_parameters.KP;   
		PI_Param->Ki_Gain = SIXSTEP_parameters.KI;      

		PI_Param->Lower_Limit_Output = LOWER_OUT_LIMIT;       
		PI_Param->Upper_Limit_Output = UPPER_OUT_LIMIT;     

		PI_Param->Max_PID_Output =  FALSE; 
		PI_Param->Min_PID_Output =  FALSE;
}


int16_t MC_PI_Controller(stSIXSTEP_PI_Param *PI_Param, int16_t speedFdbk)
{
		int32_t wProportional_Term=0, wIntegral_Term=0, wOutput_32=0,wIntegral_sum_temp=0;
		int32_t Error =0;
			
		Error = (PI_Param->Reference - speedFdbk);
		
		/* Proportional term computation*/
		wProportional_Term = PI_Param->Kp_Gain * Error;
			
		/* Integral term computation */
		if (PI_Param->Ki_Gain == 0)
		{
				SIXSTEP_parameters.integralTermSum = 0;
		}
		else
		{ 
				wIntegral_Term = PI_Param->Ki_Gain * Error;
				wIntegral_sum_temp = SIXSTEP_parameters.integralTermSum + wIntegral_Term;
				SIXSTEP_parameters.integralTermSum = wIntegral_sum_temp;
		}
  

    if(SIXSTEP_parameters.integralTermSum> KI_DIV * PI_Param->Upper_Limit_Output)
		{
       SIXSTEP_parameters.integralTermSum = KI_DIV* PI_Param->Upper_Limit_Output;
		}
   
    if(SIXSTEP_parameters.integralTermSum<-KI_DIV* PI_Param->Upper_Limit_Output)
		{
       SIXSTEP_parameters.integralTermSum = -KI_DIV* PI_Param->Upper_Limit_Output;
		}
    
   /* WARNING: the below instruction is not MISRA compliant, user should verify
               that Cortex-M3 assembly instruction ASR (arithmetic shift right) 
               is used by the compiler to perform the shifts (instead of LSR 
               logical shift right)*/ 
               
    wOutput_32 = (wProportional_Term/KP_DIV) + (SIXSTEP_parameters.integralTermSum/KI_DIV);

  
		if(PI_Param->Reference>0)
		{  
			 if (wOutput_32 > PI_Param->Upper_Limit_Output)
			 {
						wOutput_32 = PI_Param->Upper_Limit_Output;		  			 	
			 }
			 else if (wOutput_32 < PI_Param->Lower_Limit_Output)
			 {
						wOutput_32 = PI_Param->Lower_Limit_Output;
			 }
		}
		else 
		{
			 if (wOutput_32 < (- PI_Param->Upper_Limit_Output) )
			 {
						wOutput_32 = - (PI_Param->Upper_Limit_Output);		  			 	
			 }
			 else if (wOutput_32 > (-PI_Param->Lower_Limit_Output))
			 {
						wOutput_32 = (-PI_Param->Lower_Limit_Output);
			 }    
		}
		return((int16_t)(wOutput_32)); 	
}


void MC_SixStep_SetSpeed(uint16_t speed_value)
{
   if(speed_value != 0)
	 {
			PI_Parameters.Reference = speed_value;
	 } 
}

void MC_SixStep_StartMotor(void)
{ 
		SIXSTEP_parameters.status = SIXSTEP_STATUS_INIT;
}


void MC_SixStep_StopMotor(void)
{     
		//SIXSTEP_parameters.status = SIXSTEP_STATUS_STOP;
		TIM_OC_InitTypeDef commConfigOC;
	
		commConfigOC.OCMode = TIM_OCMODE_INACTIVE;
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_1); 
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_2);
		HAL_TIM_OC_ConfigChannel(&htim1, &commConfigOC, TIM_CHANNEL_3);
		MC_SixStep_Reset();
}


int32_t MC_SixStep_GetElSpeedHz(void)
{   

	//��� ����������� ������� �������� ���������
	
	 int32_t freq;
	
	 if(HALL_TIM.Instance->CCR1==0)
	 {
			freq=0;
	 }
	 else
	 {
			freq= HALL_TIM_FREQ/HALL_TIM.Instance->CCR1;
	 }
	 
   if(PI_Parameters.Reference<0)
	 {
			
			return (-freq);
	 }
   else 
	 {
			return (freq);
	 }
}


int32_t MC_SixStep_GetMechSpeedRPM(void)
{  	
		SIXSTEP_parameters.speedFdbk=(int32_t)(MC_SixStep_GetElSpeedHz() *  60 / NUM_POLE_PAIRS);
		return SIXSTEP_parameters.speedFdbk;
}


uint8_t   MC_SixStep_GetCurrentPosition(void)
{
		uint8_t HALL_1=0;
		uint8_t HALL_2=0;
		uint8_t HALL_3=0;
		uint8_t hall_position=0;
		uint8_t position=0;
	
		SIXSTEP_parameters.error=SIXSTEP_ERR_OK;
	
		HALL_1=HAL_GPIO_ReadPin(HALL_1_GPIO_Port, HALL_1_Pin);
		HALL_2=HAL_GPIO_ReadPin(HALL_2_GPIO_Port, HALL_2_Pin);
		HALL_3=HAL_GPIO_ReadPin(HALL_3_GPIO_Port, HALL_3_Pin);
	
		hall_position=((HALL_1<<2)|(HALL_2<<1)|(HALL_3));

	
		switch(hall_position)
		{
				case(0x5):
				{
						position = 6;
				}
				break;	
				
				case(0x4):
				{
						position = 5;
				}
				break;
				
				case(0x6):
				{
						position = 4;
				}
				break;	

				case(0x2):
				{
						position = 3;
				}
				break;	

				case(0x3):
				{
						position = 2;
				}
				break;	

				case(0x1):
				{
						position = 1;
				}
				break;	

				case(0x0)://fault
				{
						position = 0;
				}
				break;	

				case(0x7): //fault
				{
						position = 0;
				}
				break;					
		}
		
		if(position)
		{
				SIXSTEP_parameters.positionStep=position;
		}
		else
		{
				SIXSTEP_parameters.error=SIXSTEP_ERR_POSFBKERROR;
		}
		
		return position;
}

uint16_t MC_SixStep_GetGurrent(void)
{
		SIXSTEP_parameters.currentFdbk=500;//�����������
			
		return SIXSTEP_parameters.currentFdbk;
}


uint16_t MC_SixStep_GetVoltage(void)
{
		SIXSTEP_parameters.voltageFdbk=100;//�����������
		return SIXSTEP_parameters.voltageFdbk;
}


void MC_SixStep_InitMainData(void)
{ 
  SIXSTEP_parameters.KP = KP_GAIN;   
  SIXSTEP_parameters.KI = KI_GAIN;
  SIXSTEP_parameters.direction = DIRECTION;
	SIXSTEP_parameters.speedFdbk = 0;
	SIXSTEP_parameters.error = SIXSTEP_ERR_OK;
	SIXSTEP_parameters.status = SIXSTEP_STATUS_INIT;
	
	MC_SixStep_GetCurrentPosition();//���������� ������� ��������� ������
}


void MC_SixStep_Init(void)
{
		HAL_TIMEx_ConfigCommutationEvent_IT(&PHASE_TIM, PHASE_TIM_TRIGGER_INPUT, TIM_COMMUTATION_SOFTWARE);
		HAL_TIMEx_HallSensor_Start_IT(&HALL_TIM);

    MC_SixStep_InitMainData(); 
    MC_SixStep_Reset();
}

enSIXSTEP_Error MC_SixStep_GetParameters(void)
{
		SIXSTEP_parameters.error=SIXSTEP_ERR_OK;
	
		MC_SixStep_GetGurrent();
		if(!IS_BLDC_CURRENT(SIXSTEP_parameters.currentFdbk))
		{
				SIXSTEP_parameters.error=SIXSTEP_ERR_OVERCURRENT;
				return SIXSTEP_parameters.error;
		}
		
		MC_SixStep_GetVoltage();		
		if(!IS_BLDC_VOLTAGE(SIXSTEP_parameters.voltageFdbk))
		{
				SIXSTEP_parameters.error=SIXSTEP_ERR_OVERVOLTAGE;
				return SIXSTEP_parameters.error;
		}
		
		MC_SixStep_GetMechSpeedRPM();
		if(!IS_BLDC_RPM(SIXSTEP_parameters.speedFdbk))
		{
				SIXSTEP_parameters.error=SIXSTEP_ERR_SPEEDFBKERROR;
				return SIXSTEP_parameters.error;
		}
				
		return SIXSTEP_parameters.error;
}



void 			MC_SixStep_Handler(void)
{

	MC_SixStep_GetParameters();
	
	if(SIXSTEP_parameters.error!=SIXSTEP_ERR_OK)
	{
			SIXSTEP_parameters.status=SIXSTEP_STATUS_FAULT;
	}
	
	switch(SIXSTEP_parameters.status)
	{
			case SIXSTEP_STATUS_INIT: //���������� ���. ��������� ������
			{						
					MC_SixStep_InitMainData();
					if(SIXSTEP_parameters.error==SIXSTEP_ERR_POSFBKERROR)
					{							
							SIXSTEP_parameters.status=SIXSTEP_STATUS_FAULT;
					}
					else
					{
							SIXSTEP_parameters.status=SIXSTEP_STATUS_STOP;
					}	
			}
			break;	

			case SIXSTEP_STATUS_STOP: //
			{
					
			}
			break;			
						
			case SIXSTEP_STATUS_RUN:
			{
					SIXSTEP_parameters.PWM_Value=MC_PI_Controller(&PI_Parameters, SIXSTEP_parameters.speedFdbk);
			}
			break;
			
			case SIXSTEP_STATUS_BREAK:
			{								
					MC_SixStep_StopMotor();
					SIXSTEP_parameters.status=SIXSTEP_STATUS_STOP;
			}
			break;
			
			case SIXSTEP_STATUS_FAULT:
			{
				//error message
				
				 SIXSTEP_parameters.status=SIXSTEP_STATUS_BREAK;
			}
			break;			
			
			default:
			{
			}
			break;
	}

}



void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{			
			MC_SixStep_NextStep();
			SIXSTEP_parameters.flagIsSpeedNotZero=TRUE;		
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		if(htim->Instance == TIM2)
		{
				
			 if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			 {
					htim1.Instance->EGR|=TIM_EGR_COMG;
			 }
		}
}