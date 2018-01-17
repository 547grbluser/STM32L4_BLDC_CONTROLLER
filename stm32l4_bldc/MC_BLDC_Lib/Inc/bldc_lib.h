/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLDC_LIB_H
#define BLDC_LIB_H

#include "math.h"
#include "stm32l4xx_hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "bldc_param.h"


#define PHASE_TIM               htim1
#define PHASE_TIM_TRIGGER_INPUT TIM_TS_NONE					
#define HALL_TIM                htim2
#define SPEED_TIM								htim4	

#define HALL_TIM_FREQ						20000 //Hz


typedef enum 
{
    SIXSTEP_STATUS_IDLE=0,
		SIXSTEP_STATUS_INIT,
		SIXSTEP_STATUS_RAMP,	
    SIXSTEP_STATUS_RUN,
	  SIXSTEP_STATUS_BREAK, 
	  SIXSTEP_STATUS_STOP, 
		SIXSTEP_STATUS_FAULT, 	
		
} enSIXSTEP_SystStatus;


typedef enum
{
	 SIXSTEP_ERR_OK=0,
	 SIXSTEP_ERR_SPEEDFBKERROR, 
	 SIXSTEP_ERR_POSFBKERROR,
   SIXSTEP_ERR_OVERCURRENT,
	 SIXSTEP_ERR_OVERVOLTAGE, 
   SIXSTEP_ERR_STARTUP_FAILURE,                   	
} enSIXSTEP_Error;

typedef enum
{
		SIXSTEP_DIR_FORWARD=0,
		SIXSTEP_DIR_BACKWARD,
}enSIXSTEP_Direction;

/** 
  * @brief  Six PI regulator parameters  
  */

typedef struct
{
  uint16_t ReferenceSpeed;                    /*!< Refence value for PI regulator */ 
  int16_t Kp_Gain;                      /*!< Kp value for PI regulator */ 
  int16_t Ki_Gain;                      /*!< Ki value for PI regulator */  
  int16_t Lower_Limit_Output;           /*!< Min output value for PI regulator */ 
  int16_t Upper_Limit_Output;           /*!< Max output value for PI regulator */ 
  int8_t 	Max_PID_Output;                /*!< Max Saturation indicator flag */ 
  int8_t 	Min_PID_Output;                /*!< Min Saturation indicator flag */ 
	
	int32_t 	integralTermSum;             /*!< Global Integral part for PI*/  
} stSIXSTEP_PI_Param;  /*!< PI Data Structure */


/** 
  * @brief  Six Step parameters  
  */
typedef struct
{
	enSIXSTEP_SystStatus status;      			/*!< Status variable for SixStep algorithm*/ 
	enSIXSTEP_Error			 error;							/*!< Error variable for SixStep algorithm*/ 

	/*BLDC read params*/
	uint16_t 	speedFdbk;
	uint8_t  	flagIsSpeedNotZero;
	uint16_t 	currentFdbk;
	uint16_t 	voltageFdbk;
	uint8_t 	positionFdbk;
	
	/*BLDC write params*/
//	int16_t 	speedTarget;
	enSIXSTEP_Direction		direction;
	uint8_t		positionStep;
	uint16_t 	PWM_Value;
	
	stSIXSTEP_PI_Param PI_Param;

}  stSIXSTEP_Base;             /*!< Six Step Data Structure */





void MC_SixStep_Init(void);
void MC_SixStep_StartMotor(void);
void MC_SixStep_StopMotor(void);
void MC_SixStep_SetSpeed(uint16_t);
void MC_SixStep_Handler(void); 
#endif
