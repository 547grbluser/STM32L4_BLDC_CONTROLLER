#include "protocol.h"

//
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


//****************************************************************************
// ������� ����������


extern const CommandTblStruct cmdTblCOMP[];
extern const CommandTblStruct cmdTblAD[];
extern const CommandTblStruct cmdTblDC[];
extern const CommandTblStruct cmdTblCTRL[];

// ������� �������
//****************************************************************************
// ���������� ���������� ��������� �����
const CommandTblStruct *pCmdList[TLS_MAX_ADDR_CNT]={
	NULL,	// 0
	&cmdTblCOMP[0],	// 1
	NULL,	// 2
	NULL,	//&cmdTblAD[0],	// 3
	NULL,	// 4
	NULL,	// 5
	&cmdTblDC[0],	//cmdTblDC,	// 6
	NULL,	//cmdTblCTRL,	// 7
	NULL,	// 8
	NULL,	// 9
};


//****************************************************************************
/*
*/
uint8_t cmdGetLen(uint8_t cntParam)
{
	switch (cntParam){
		case 0:
			return LEN_CMD_NO_PARAM;
		case 1:
			return LEN_CMD_ONE_PARAM;
		case 2:
			return LEN_CMD_TWO_PARAM;
		case 3:
			return LEN_CMD_THREE_PARAM;
		case 4:
			return LEN_CMD_FOUR_PARAM;
		default:
			return LEN_CMD_NO_PARAM;
	}
}

/* 
����� ������� ������
*/
void cmdResetReceive(stCmd *pCmd)
{
	pCmd->rcvByteCnt = 0;	
	pCmd->state = CMD_RCV_RESET;
}

/*
���������� ������� ������� ����� �������. ��� ������������� ��������� ���������������� ����� �������������� � ����� ����� �������
*/
void cmdReady(stCmd *pCmd)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the TLS_cmdReady can be implemented in the user file
   */ 
	const CommandTblStruct *pCmdTbl;
	pCmdTbl = pCmdList[pCmd->addr];
	if(pCmdTbl[pCmd->cmd].pCommFunc != NULL){
		pCmdTbl[pCmd->cmd].pCommFunc(pCmd->Param, pCmd->from);		//������� ���������� �������
	}
	cmdResetReceive(pCmd); // ����� ��������� ���, ������ ������� �������
}

/*
*/
static void cmdPutCharToBuf(stCmd *pCmd, char c)
{
	pCmd->RxCmdBuf[pCmd->rcvByteCnt] = c;
	if(pCmd->rcvByteCnt < RX_CMD_BUFSIZE-1){	// ��������� �� ������ ������
		pCmd->rcvByteCnt++;
	}
}
//****************************************************************************
// ��������� ������ �� �������� ASCII '0'...'9', 'A'...'F'
// � ������������ ��� � ���������� �������� �� 0 �� 15 � ���������� ���.
// ���� ������ �� � ��������� ���������� -1
int8_t cmdCnvChar16(char Src)
{
	if((Src >= '0') && (Src <= '9')){
		return (Src - '0');
	}
	if((Src >= 'A') && (Src <= 'F')){
		return (Src - 'A' + 10);
	}
	return (-1);
}

//****************************************************************************
// ���������� ��������� � ������� ASCII xxxx � �������� ������� ������
// � ��������������� ��� � int
// ���� ������� ���������� �������� �������, ����� NULL
char* cmdCnvParam (char* pSrc, uint16_t *Value)
{
	int8_t Nibble;
	uint8_t i;
	for(i = 0; i < 4; i++, pSrc++){
		Nibble = cmdCnvChar16(*pSrc);
		if(Nibble < 0)
			return (NULL);
		*Value <<= 4;
		*Value |= Nibble;
	}
	return (pSrc);
}

//****************************************************************************
// �������� ������������ ���������� � ������� � ��������������� ��
// � ����� �������� � ����������� ��������������� � ��������� �������
// ���� ��������� - ��������� 0
// � ������ ������ ���������� -��� ������:
// -1 - ����������� '\n'
// -2  �������� ��������
int cmdCheckAndConvertParam(stCmd *pCmd)
{
	char* pBuf = &pCmd->RxCmdBuf[1];	// ��������� ���������� � ������ � ������� 
	const CommandTblStruct *pCmdTbl; 
	pCmdTbl = pCmdList[pCmd->addr];
	for (uint8_t i=0; i<pCmdTbl[pCmd->cmd].paramCnt; i++){
		if(i>0){	//����� ����������� ������ ���� ' '
			if(*pBuf != ' '){	// error
				return (-2);
			}
			pBuf++;
		}
		pBuf = CnvParam (pBuf, &pCmd->Param[i]);
		if(pBuf < 0){	// error
			return (-2);
		}	
	}
	if(*pBuf != '\r'){	// error
		return (-1);
	}
	return (0);
}



/******************************************************************************
��������� ����� ������ ����������, ������������� � ����� ������� ����������
���������� � ��������� ������ ������� ����������� � ��������� (������� ��������������)

���������:
��������� �� ��������� �������� ��������� ������ �������
�������� ������

*/

#define CMD_START_CHAR	':'
void cmdRxChar (stCmd *pCmd, unsigned char c) 
{
  char i;
  const CommandTblStruct *pCmdTbl;
  if(c == CMD_START_CHAR)
	{					// ������ �����
	   cmdResetReceive(pCmd);		// ������� ����� (����� �� ���?)
		
//	   if((c == ADDR_TLS_COMP || c == ADDR_TLS_ADC || c == ADDR_TLS_DC || c == ADDR_TLS_CTRL))
//		 { // ������� ��� ����� - ������ ������ �������
//					pCmd->state=RCV_TLS_ADDR;
//					pCmd->addr = c;
//		//			putCharToBuf(pCmd, c);
//					if(getErrorExist(pCmd->addr) != 0)
//					{
//						 c |= 0x80;				// ��������� ��� ������� ������
//					}
//					fputc(c, pCmd->from);			// �������� ���
//		 }
		
			/*
				�������� ���
			*/
	}
	else
	{		// ����� ������ ������� (�� �����)
		switch (pCmd->state)
		{
			case CMD_RCV_ADDR:
			{
				if((c >= 'A') && (c <= 'Z'))
				{
						cmdPutCharToBuf(pCmd, c);
						c -= 'A';
						pCmd->cmd=c;
						pCmdTbl = pCmdList[pCmd->addr];
						pCmd->len = cmdGetLen(pCmdTbl[c].paramCnt);		// ��������� ����� �������
					
						if(pCmdTbl[c].pCommFunc == NO_ENTRY)
						{
//								fprintf(pCmd->from, "%s", ErrUnknownCmd);	// Err0
								cmdResetReceive(pCmd);
						}
						else
						{
		//						pCmd->rcvByteCnt = 1;
								pCmd->state=CMD_RCV_PARAM;
						}
				}
				else
				{		// �������� ���
//						fprintf(pCmd->from, "%s", ErrUnknownCmd);	// Err0
						cmdResetReceive(pCmd);
				}
			}
			break;
				
			case CMD_RCV_PARAM:
			{
					cmdPutCharToBuf(pCmd, c);
					if(pCmd->rcvByteCnt == pCmd->len)
					{	// ������� ������� (�� �����)
							pCmdTbl = pCmdList[pCmd->addr];
							if(cmdCheckAndConvertParam(pCmd) == 0)
							{		//parameters OK!
									for(i = 1; i < pCmd->len; i++)
									{
											fputc(pCmd->RxCmdBuf[i], pCmd->from);			// �������� ���
									}
									pCmd->state=CMD_RCV_COMPLETE;
									cmdReady(pCmd);
							}
							else
							{	// parameters error
//									fprintf(pCmd->from, "%s", ErrFaultyParam);	// Err0
									cmdResetReceive(pCmd);
							}
					}
			}
			break;
			
			default:	// � ������ ���������� ������ �� ���������
			{
			}
			break;
		}
	}
}


