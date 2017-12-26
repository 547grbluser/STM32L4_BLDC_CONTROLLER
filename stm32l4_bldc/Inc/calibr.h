#ifndef CALIBR_H
#define CALIBR_H
#include "main.h"

typedef struct
{
		float k;
		float b;
} stCalibrCoefs;

typedef struct 
{
	uint16_t pointVoltage_1;
	float 	 pointValue_1;
	uint16_t pointVoltage_2;
	float 	 pointValue_2;
} stCalibrValues;

#endif
