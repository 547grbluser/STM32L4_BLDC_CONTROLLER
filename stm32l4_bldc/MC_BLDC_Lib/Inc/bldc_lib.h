/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLDC_LIB_H
#define BLDC_LIB_H

#include "math.h"
#include "stm32l4xx_hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "bldc_param.h"
#include "bldc_pid.h"


#define PHASE_TIM               htim1
#define PHASE_TIM_TRIGGER_INPUT TIM_TS_NONE					
#define HALL_TIM                htim2
#define SPEED_TIM								htim4	

#define HALL_TIM_FREQ						80000 //Hz


typedef enum 
{
    SIXSTEP_STATUS_IDLE=0,
		SIXSTEP_STATUS_INIT,
		SIXSTEP_STATUS_PREV_STEP,
		SIXSTEP_STATUS_RAMP,	
    SIXSTEP_STATUS_RUN,
	  SIXSTEP_STATUS_BREAK, 
	  SIXSTEP_STATUS_STOP, 
		SIXSTEP_STATUS_FAULT, 
		SIXSTEP_STATUS_RESTART,
		SIXSTEP_OPEN_VT_SWITCH,
		
} enSIXSTEP_SystStatus;


typedef enum
{
	 SIXSTEP_ERR_OK											=	0x00,
//	 SIXSTEP_ERR_SPEEDFBKERROR					=	0x01, 
	 SIXSTEP_ERR_POSFBKERROR						=	0x01,			
   SIXSTEP_ERR_OVERCURRENT						= 0x02,
//	 SIXSTEP_ERR_OVERVOLTAGE						= 0x08, 
//   SIXSTEP_ERR_STARTUP_FAILURE				= 0x10, 
	 SIXSTEP_ERR_UNEXPECTED_STOP				= 0x04,
	 SIXSTEP_ERR_HALL_SENSOR_ERROR			=	0x08,	
} enSIXSTEP_Error;

typedef enum
{
		SIXSTEP_DIR_FORWARD=0,
		SIXSTEP_DIR_BACKWARD,
}enSIXSTEP_Direction;

typedef enum
{
		SIXSTEP_MODE_MOTOR=0,
		SIXSTEP_MODE_OPEN_SWITCH,
}enSIXSTEP_Mode;

/** 
  * @brief  Six Step parameters  
  */
typedef struct
{
	enSIXSTEP_SystStatus status;      			/*!< Status variable for SixStep algorithm*/ 
	enSIXSTEP_Error			 error;							/*!< Error variable for SixStep algorithm*/ 
	uint8_t					faultCnt;


	uint16_t 	speedFdbk;
	uint8_t  	flagIsSpeedNotZero;
	uint16_t 	currentFdbk;
	uint16_t 	voltageFdbk;
	uint8_t 	positionFdbk;
	uint32_t 	phaseCounter;
	

	uint8_t 	prevStep;
	enSIXSTEP_Direction		direction;
	enSIXSTEP_Mode	mode;
	uint8_t		positionStep;
	uint16_t 	PWM_Value;
	
	stSIXSTEP_PI_Param PI_Param;

}  stSIXSTEP_Base;             /*!< Six Step Data Structure */





void MC_SixStep_Init(void);
void MC_SixStep_StartMotor(enSIXSTEP_Direction dir);
void MC_SixStep_StopMotor(void);
uint16_t MC_SixStep_GetCurrent(void);
uint16_t MC_SixStep_GetVoltage(void); 
uint32_t MC_SixStep_GetPhaseCounter(void);
uint32_t MC_SixStep_GetMechSpeedRPM(void);
enSIXSTEP_SystStatus MC_SixStep_GetStatus(void); 
void MC_SixStep_SetSpeed(uint16_t);
enSIXSTEP_Error	MC_SixStep_GetErrorCode(void);
void MC_SixStep_Handler(void); 

/*
	Режим теста кабеля
*/
void 			MC_SixStep_OpenVTSwitch(void);	
void 			MC_SixStep_CloseVTSwitch(void);
#endif
