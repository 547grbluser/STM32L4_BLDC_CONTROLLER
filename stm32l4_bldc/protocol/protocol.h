#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdio.h>

#define N_COMMAND					(26)	// ���������� �������������� ������ ('A'...'Z')
#define TLS_MAX_ADDR_CNT	(10)

#define ADDR_TLS_COMP	(1)
#define ADDR_TLS_ADC	(3)
#define ADDR_TLS_DC		(6)
#define ADDR_TLS_CTRL	(7)

#define NO_ENTRY				NULL

#define RX_CMD_BUFSIZE	(256)

// ��������� ��� �������� ������ ������ ���������� � �� �����
typedef struct{
	int (*pCommFunc) (uint16_t* param, FILE*);
	uint8_t paramCnt;		// ����� ������� � ������ (������� ��� � '\n')
}CommandTblStruct;

//�������� ��� ���� ����� �������:

#define LEN_CMD_NO_PARAM		(2)		//����� ������� ��� ����������
#define LEN_CMD_ONE_PARAM		(6)		//����� ������� � ����� ����������
#define LEN_CMD_TWO_PARAM		(11)		//����� ������� � ����� �����������
#define LEN_CMD_THREE_PARAM		(16)		//����� ������� � ����� �����������
#define LEN_CMD_FOUR_PARAM		(21)		//����� ������� � ����� �����������


// ��������� ��� �������� �������� ������� �� ���
#define MAX_N_PAR	4
 
/*typedef struct{
	uint8_t addr;
	char (*pCommFunc) (void);		// ������� - ����������
	uint16_t Param[MAX_N_PAR];		// ������ ����������
}CommandStruct;*/

typedef enum {
	CMD_RCV_RESET,
	CMD_RCV_ADDR,
	CMD_RCV_PARAM,
	CMD_RCV_ADDITIONAL_DATA,
	CMD_RCV_COMPLETE,
} encCmdRcvState;

// ��������� ��������� ������ �������
typedef struct {
	encCmdRcvState state;		// 
	char RxCmdBuf[RX_CMD_BUFSIZE];	// ����� ����� �������
	uint8_t len;					// ����� ����������� ������� (������� '\n')
	uint8_t rcvByteCnt;				// ���������� �������� ������ �������
	uint8_t addr;
	uint8_t cmd;						// ��� ������� (index in table)
	FILE* from;						// ������ �� ��������� ������
	uint16_t Param[MAX_N_PAR];		// ������ ����������
} stCmd;

void cmdRxChar (stCmd *pCmd, unsigned char c);
char* CnvParam (char* pSrc, uint16_t *Value);



#endif