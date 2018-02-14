#define NUM_POLE_PAIRS                       2      /*!< Number of Motor Pole pairs */
#define	PAIR								 		 2
#define BLDC_SIX_STEP						 6

#define DIRECTION                         SIXSTEP_DIR_FORWARD      /*!< Set motor direction CW = 0 and CCW = 1*/ 
#define MC_TARGET_SPEED                      3000      /*!< Target speed in closed loop control when the potentiometer is disabled */  

#define BLDC_CURRENT_MIN												0					//mA
#define BLDC_CURRENT_MAX												4000			//mA
#define IS_BLDC_CURRENT(__CURRENT__) (((__CURRENT__) >=BLDC_CURRENT_MIN) && ((__CURRENT__) <= BLDC_CURRENT_MAX))

#define BLDC_VOLTAGE_MIN												20					//V
#define BLDC_VOLTAGE_MAX												700					//V
#define IS_BLDC_VOLTAGE(__VOLTAGE__) (((__VOLTAGE__) >=BLDC_VOLTAGE_MIN) && ((__VOLTAGE__) <= BLDC_VOLTAGE_MAX))


#define BLDC_RPM_MIN													  0					//RPM
#define BLDC_RPM_MAX														5000			//RPM
#define IS_BLDC_RPM(__RPM__) (((__RPM__) >=BLDC_RPM_MIN) && ((__RPM__) <= BLDC_RPM_MAX))


#define BLDC_PWM_START															0
#define BLDC_PWM_RAMP_MAX														60	

#define BLDC_FAULT_RESTART_N												5

/*
	Коэффициенты аналоговых каналов
*/

#define MC_CURRENT_COEF 		5/2
#define MC_VOLTAGE_COEF 		100
#define MC_VOLTAGE_MULTIPLIER	20//для совместимости


#define DUTY_CYCLE_INIT_VALUE                 1     /*!< Initial duty cycle value during startup */          

#define BLDC_CHARGE_CAP_TIME									5//ms

#ifndef TRUE
#define TRUE                                 1      /*!< Define TRUE */  
#endif

#ifndef FALSE
#define FALSE                                0      /*!< Define FALSE */
#endif
