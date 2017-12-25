#define NUM_POLE_PAIRS                       7      /*!< Number of Motor Pole pairs */
#define DIRECTION                         SIXSTEP_DIR_FORWARD      /*!< Set motor direction CW = 0 and CCW = 1*/ 
#define TARGET_SPEED                      3000      /*!< Target speed in closed loop control when the potentiometer is disabled */  

#define BLDC_CURRENT_MIN												0					//mA
#define BLDC_CURRENT_MAX												4000			//mA
#define IS_BLDC_CURRENT(__CURRENT__) (((__CURRENT__) >=BLDC_CURRENT_MIN) && ((__CURRENT__) <= BLDC_CURRENT_MAX))

#define BLDC_VOLTAGE_MIN												50					//V
#define BLDC_VOLTAGE_MAX												700					//V
#define IS_BLDC_VOLTAGE(__VOLTAGE__) (((__VOLTAGE__) >=BLDC_VOLTAGE_MIN) && ((__VOLTAGE__) <= BLDC_VOLTAGE_MAX))


#define BLDC_RPM_MIN														0					//RPM
#define BLDC_RPM_MAX														5000			//RPM
#define IS_BLDC_RPM(__RPM__) (((__RPM__) >=BLDC_RPM_MIN) && ((__RPM__) <= BLDC_RPM_MAX))

#define BLDC_ACCEL_MIN														0					//RPM/M
#define BLDC_ACCEL_MAX														500				//RPM/M
#define IS_BLDC_ACCEL(__ACCEL__) (((__ACCEL__) >=BLDC_ACCEL_MIN) && ((__ACCEL__) <= BLDC_ACCEL_MAX))




#define KP_GAIN		                          100     /*!< Kp parameter for PI regulator */
#define KI_GAIN		                           30     /*!< Ki parameter for PI regulator */   
#define KP_DIV  	                         8192     /*!< Kp parameter divider for PI regulator */
#define KI_DIV	                           8192     /*!< Ki parameter divider for PI regulator */   
#define LOWER_OUT_LIMIT		                 	400     /*!< Low Out value of PI regulator */      
#define UPPER_OUT_LIMIT		                 1799     /*!< High Out value of PI regulator */   
#define DUTY_CYCLE_INIT_VALUE               199     /*!< Initial duty cycle value during startup */          

#define TRUE                                 1      /*!< Define TRUE */  
#define FALSE                                0      /*!< Define FALSE */
