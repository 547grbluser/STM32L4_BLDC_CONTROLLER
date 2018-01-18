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
#define _CMD_HELP   "help"
#define _CMD_CLEAR  "clear"
//#define _CMD_CLR    "clear_port"
//#define _CMD_SET    "set_port"

#define _CMD_BLDC_START_FWD "start_fwd"
#define _CMD_BLDC_START_BWD "start_bwd"
#define _CMD_BLDC_STOP 			"stop"

#define _CMD_BLDC_GET_ERR "get_err"
#define _CMD_BLDC_GET_CURRENT "get_current"
#define _CMD_BLDC_GET_VOLTAGE "get_voltage"


//#define _CMD_TEST_MODE 	"test_mode"
//#define _CMD_TEST_OUT		"test_out"
//#define _CMD_START			"start"
//#define _CMD_STOP				"stop"
//#define _CMD_PULSE_TIME "pulse_time"
//#define _CMD_CYCLE_TIME	"cycle_time"
//#define _CMD_LOAD_PATTERN	"load_pattern"

	
	//arguments _CMD_TEST_MODE
//	#define _ACMD_TEST_MODE_ON  "on"
//	#define _ACMD_TEST_MODE_OFF "off"
	


//#define _NUM_OF_CMD 9
//#define _NUM_OF_SETCLEAR_SCMD 2

//available  commands
//char * keywords [] = {_CMD_HELP, _CMD_CLEAR, _CMD_TEST_MODE, _CMD_TEST_OUT, _CMD_START, _CMD_STOP, _CMD_PULSE_TIME, _CMD_CYCLE_TIME, _CMD_LOAD_PATTERN};
//// 'set/clear' command argements
//char * set_clear_key [] = {_SCMD_PB, _SCMD_PD};

// array for completion
//char * compl_word [_NUM_OF_CMD + 1];

#define CMD_IFACE_TASK_STACK_SIZE  256

microrl_t rl;
microrl_t * prl = &rl;
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
//		chr = get_char();
		
		
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
				print("Start motor forward\r\n");
				MC_SixStep_StartMotor(SIXSTEP_DIR_FORWARD);
		}
		else if (strcmp (argv[i], _CMD_BLDC_START_BWD) == 0) 
		{
				print("Start motor backrward\r\n");
				MC_SixStep_StartMotor(SIXSTEP_DIR_BACKWARD);
		}	
		else if (strcmp (argv[i], _CMD_BLDC_STOP) == 0) 
		{
				print("Stop motor\r\n");
				MC_SixStep_StopMotor();
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
				print ("command: '");
				print ((char*)argv[i]);
				print ("' Not found.\n\r");
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