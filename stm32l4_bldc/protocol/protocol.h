#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdio.h>

#define N_COMMAND					(26)	// количество обрабатываемых команд ('A'...'Z')
#define TLS_MAX_ADDR_CNT	(10)

#define ADDR_TLS_COMP	(1)
#define ADDR_TLS_ADC	(3)
#define ADDR_TLS_DC		(6)
#define ADDR_TLS_CTRL	(7)

#define NO_ENTRY				NULL

#define RX_CMD_BUFSIZE	(256)

// структура дл€ хранени€ списка команд устройства и их длины
typedef struct{
	int (*pCommFunc) (uint16_t* param, FILE*);
	uint8_t paramCnt;		// длина команды в байтах (включа€  ќѕ и '\n')
}CommandTblStruct;

//значени€ дл€ пол€ длины команды:

#define LEN_CMD_NO_PARAM		(2)		//длина команды без параметров
#define LEN_CMD_ONE_PARAM		(6)		//длина команды с одним параметром
#define LEN_CMD_TWO_PARAM		(11)		//длина команды с двум€ параметрами
#define LEN_CMD_THREE_PARAM		(16)		//длина команды с трем€ параметрами
#define LEN_CMD_FOUR_PARAM		(21)		//длина команды с трем€ параметрами


// —труктура дл€ хранени€ прин€той команды от “Ћ—
#define MAX_N_PAR	4
 
/*typedef struct{
	uint8_t addr;
	char (*pCommFunc) (void);		// функци€ - обработчик
	uint16_t Param[MAX_N_PAR];		// массив параметров
}CommandStruct;*/

typedef enum {
	CMD_RCV_RESET,
	CMD_RCV_ADDR,
	CMD_RCV_PARAM,
	CMD_RCV_ADDITIONAL_DATA,
	CMD_RCV_COMPLETE,
} encCmdRcvState;

// структура состо€ни€ приема команды
typedef struct {
	encCmdRcvState state;		// 
	char RxCmdBuf[RX_CMD_BUFSIZE];	// буфер сбора команды
	uint8_t len;					// длина принимаемой команды (включа€ '\n')
	uint8_t rcvByteCnt;				// количество прин€тых байтов команды
	uint8_t addr;
	uint8_t cmd;						// код команды (index in table)
	FILE* from;						// ссылка на интерфейс приема
	uint16_t Param[MAX_N_PAR];		// массив параметров
} stCmd;

void cmdRxChar (stCmd *pCmd, unsigned char c);
char* CnvParam (char* pSrc, uint16_t *Value);



#endif