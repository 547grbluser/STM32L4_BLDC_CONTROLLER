#include "cmd_iface.h"
#include "stm32l4xx_hal.h"

#include <stdio.h>
#include <string.h>
#include "microrl.h"
#include "bldc_lib.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "fifo.h"

// definition commands word
#define _CMD_HELP   "HLP"
#define _CMD_CLEAR  "CLR"
#define _CMD_ECHO		"ECHO"


//#define _CMD_BLDC_START_FWD 	"start_fwd"
//#define _CMD_BLDC_START_BWD 	"start_bwd"
//#define _CMD_BLDC_STOP 				"stop"


#define _CMD_BLDC_GET_CURRENT 		"get_current"
#define _CMD_BLDC_GET_VOLTAGE 		"get_voltage"
#define _CMD_BLDC_GET_RPM_SPEED 	"get_rpm"
#define _CMD_BLDC_GET_STATUS 			"get_status"
#define _CMD_BLDC_GET_ERROR_CODE 	"get_err"



#define _CMD_BLDC_START_FWD 				"STRTR"
#define _CMD_BLDC_START_BWD 				"STRTL"
#define _CMD_BLDC_STOP 							"STP"
#define _CMD_BLDC_OPEN_VT_SWITCH 		"OPNSW"
#define _CMD_BLDC_GET_PARAMS 				"PRM"

#define _CMD_BLDC_CMD_ERROR					"ERR 1\n\r"


#define _CMD_ON		"ON"
#define _CMD_OFF	"OFF"
	


#define CMD_IFACE_TASK_STACK_SIZE  256

microrl_t rl;
microrl_t * prl = &rl;
uint8_t cmdEchoFlag	= 0;

extern UART_HandleTypeDef huart1;
xSemaphoreHandle xUARTSemaphore;
#define UART_FIFO_SIZE	256
FIFO(UART_FIFO_SIZE) uart1_rx_fifo;

static void Cmd_Iface_Task(void *pvParameters);

void print (const char * str);
char get_char (void) ;
void put_char (unsigned char ch);
void print_help (void);
int execute (int argc, const char * const * argv);
char ** complet (int argc, const char * const * argv);
void sigint (void);

void Cmd_Iface_Init(void)
{
	vSemaphoreCreateBinary(xUARTSemaphore);
	microrl_init (prl, print);
	microrl_set_execute_callback (prl, execute);

#ifdef _USE_COMPLETE
	microrl_set_complete_callback (prl, complet);
#endif
	// set callback for Ctrl+C
	//microrl_set_sigint_callback (prl, sigint);	
	
	xTaskCreate(Cmd_Iface_Task,"CMD interface task",CMD_IFACE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}


static void Cmd_Iface_Task(void *pvParameters)
{
	uint8_t chr;
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	while(1)
	{		
		if(!FIFO_IS_EMPTY( uart1_rx_fifo ))
		{
				chr = FIFO_FRONT( uart1_rx_fifo );
				FIFO_POP( uart1_rx_fifo );
				microrl_insert_char (prl, chr);	
		}
	}
}

void Cmd_UART_Rx(UART_HandleTypeDef *huart)
{
	  char ch;

		ch = huart->Instance->RDR;
		if( !FIFO_IS_FULL( uart1_rx_fifo ) ) 
		{
			FIFO_PUSH( uart1_rx_fifo, ch );
		}
}


//*****************************************************************************
void print (const char * str)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),100);
}

//*****************************************************************************
char get_char (void) 
{
	uint8_t chr;
	HAL_UART_Receive(&huart1,(uint8_t *)&chr,1,0xFFFFFFFF);
	return chr;
}

//*****************************************************************************
void print_help (void)
{
	print ("\tclear               - clear screen\n\r");
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (int argc, const char * const * argv)
{
	int i = 0;
	static uint8_t str[128];
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) 
		{

				print ("microrl v");
				print (MICRORL_LIB_VER);
				print("\n\r");
				print_help ();        // print help
		} 
		else if (strcmp (argv[i], _CMD_CLEAR) == 0) 
		{
				print ("\033[2J");    // ESC seq for clear entire screen
				print ("\033[H");     // ESC seq for move cursor at left-top corner
		}
		else if (strcmp (argv[i], _CMD_BLDC_START_FWD) == 0) 
		{
				uint16_t err = MC_SixStep_GetErrorCode();	
				sprintf(str, "OK %02X\r\n", err);			
				print(str);
				MC_SixStep_StartMotor(SIXSTEP_DIR_FORWARD);
		}
		else if (strcmp (argv[i], _CMD_BLDC_START_BWD) == 0) 
		{
				uint16_t err = MC_SixStep_GetErrorCode();
				sprintf(str, "OK %02X\r\n", err);			
				print(str);
				MC_SixStep_StartMotor(SIXSTEP_DIR_BACKWARD);
		}	
		else if (strcmp (argv[i], _CMD_BLDC_STOP) == 0) 
		{
				uint16_t err = MC_SixStep_GetErrorCode();
				sprintf(str, "OK %02X\r\n", err);			
				print(str);
				MC_SixStep_StopMotor();
		}		
		else if (strcmp (argv[i], _CMD_BLDC_GET_ERROR_CODE) == 0) 
		{
				uint16_t err = MC_SixStep_GetErrorCode();
				
				sprintf(str, "Error code = %X\r\n", err);			
				print(str);
		}		
		else if (strcmp (argv[i], _CMD_BLDC_GET_CURRENT) == 0) 
		{
				uint16_t current = MC_SixStep_GetCurrent();
				sprintf(str, "Current = %d mA\r\n", current);			
				print(str);
		}	
		else if (strcmp (argv[i], _CMD_BLDC_GET_VOLTAGE) == 0) 
		{
				uint16_t voltage = MC_SixStep_GetVoltage();
				sprintf(str, "Voltage = %d V\r\n", voltage);			
				print(str);
		}			
		else if (strcmp (argv[i], _CMD_BLDC_GET_STATUS) == 0) 
		{
				uint16_t status = MC_SixStep_GetStatus();
				sprintf(str, "Status code = %d\r\n", status);			
				print(str);
		}
		else if (strcmp (argv[i], _CMD_BLDC_GET_RPM_SPEED) == 0) 
		{
				uint16_t rpm = (uint16_t)MC_SixStep_GetMechSpeedRPM();
				sprintf(str, "RPM = %d\r\n", rpm);			
				print(str);
		}	
		else if(strcmp (argv[i], _CMD_BLDC_GET_PARAMS) == 0) 
		{
				
				uint16_t err = MC_SixStep_GetErrorCode();	
				uint16_t current = MC_SixStep_GetCurrent();
				uint16_t voltage = MC_SixStep_GetVoltage();
			  uint16_t rpm = (uint16_t)MC_SixStep_GetMechSpeedRPM();
				
				uint16_t phase = 0;
			
				sprintf(str, "OK %02X %d %d %d %d\r\n", err, voltage, current, rpm, phase);			
				print(str);			
		}
		else if (strcmp (argv[i], _CMD_BLDC_OPEN_VT_SWITCH) == 0) 
		{
				uint16_t err = MC_SixStep_GetErrorCode();
				sprintf(str, "OK %02X\r\n", err);			
				print(str);
				/*
					Open VT SWITCH
				*/
				MC_SixStep_OpenVTSwitch();
		}
		else if (strcmp (argv[i], _CMD_ECHO) == 0) 
		{
				if (++i < argc)
				{
						if (strcmp (argv[i], _CMD_ON) == 0) 
						{
								cmdEchoFlag = 1;
						}
						else if(strcmp (argv[i], _CMD_OFF) == 0)
						{
								cmdEchoFlag = 0;
						}
				}
				sprintf(str, "OK\r\n");			
				print(str);
		}	

//		else if ((strcmp (argv[i], _CMD_SET) == 0) || 
//							(strcmp (argv[i], _CMD_CLR) == 0)) 
//		{
//				if (++i < argc)
//				{
//					int val = strcmp (argv[i-1], _CMD_CLR);
//					unsigned char * port = NULL;
//					int pin = 0;
//					if (strcmp (argv[i], _SCMD_PD) == 0) {
//					//	port = (unsigned char *)&PORTD;
//					} else if (strcmp (argv[i], _SCMD_PB) == 0) {
//						//port = (unsigned char *)&PORTB;
//					} else {
//						print ("only '");
//						print (_SCMD_PB);
//						print ("' and '");
//						print (_SCMD_PD);
//						print ("' support\n\r");
//						return 1;
//					}
//					if (++i < argc) {
//						pin = atoi (argv[i]);
//					//	set_port_val (port, pin, val);
//						return 0;
//					} else {
//						print ("specify pin number, use Tab\n\r");
//						return 1;
//					}
//				} 
//				else 
//				{
//						print ("specify port, use Tab\n\r");
//					return 1;
//				}
//		} 
		else 
		{
//				print ("command: '");
//				print ((char*)argv[i]);
//				print ("' Not found.\n\r");
					print (_CMD_BLDC_CMD_ERROR);
		}
		i++;
	}
	return 0;
}



//*****************************************************************************
void sigint (void)
{
	print ("^C catched!\n\r");
}