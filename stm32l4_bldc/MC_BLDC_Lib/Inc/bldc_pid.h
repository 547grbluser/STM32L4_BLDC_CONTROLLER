#ifndef BLDC_PID_H
#define BLDC_PID_H

#include "stm32l4xx_hal.h"

#define KP_GAIN		                          	1//100     /*!< Kp parameter for PI regulator */
#define KI_GAIN		                           	0//30     /*!< Ki parameter for PI regulator */   
#define KP_DIV  	                         		8192     /*!< Kp parameter divider for PI regulator */
#define KI_DIV	                           		8192     /*!< Ki parameter divider for PI regulator */   
#define LOWER_OUT_LIMIT		                 		1     /*!< Low Out value of PI regulator */      
#define UPPER_OUT_LIMIT		                 	 	63     /*!< High Out value of PI regulator */   

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

uint16_t 	MC_SixStep_PI_Controller(stSIXSTEP_PI_Param *, uint16_t);
void 			MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *);

#endif