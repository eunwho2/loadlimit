/*
 * global.h
 *
 * Created: 6/28/2020 8:04:37 AM
 * Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

const uint8_t fndTableChar[26] ={0x77,0x7c,0x39,0x5e,0x79,0x71,0x7d,0x76,0x06,0x0e,0x78,0x38,
				0x55,0x54,0x5c,0x73,0xdc,0x50,0x6d,0x78,0x1c,0x1c,0x1d,0x64,0x66,0x49};
const uint8_t fndTableNum[] =	{0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x67};
const uint8_t fndTableDigitSelect[] = {0x08, 0x04, 0x02, 0x01};

int16_t codePoint;
int16_t codeFilt;
int16_t codeDivision;
int16_t codeAutoZero;
int16_t codeAlarm;
int16_t codeOver;
int16_t codeWeight;
int16_t codeAdcSpan;
int16_t codeAdcZero;

int tripNumber;

volatile int x;

volatile uint32_t timerCounter2 = 0;
volatile uint32_t cntModeErr = 0;

volatile unsigned int FndValue = 0;

volatile uint8_t machineState;
volatile uint8_t mStateBk;

uint8_t blinkCmdFlag; // bit 0 : digit 1 blinking    bit 1 : digit 10 blinking
uint8_t fndData[4];

uint8_t command;

uint8_t passWord[4];
uint8_t  editDigitPoint;		// digit for change
int16_t adcWeight;

int codeNumber;
int16_t tmpCodeData;

int codeNumberBk;
int16_t tmpCodeDataBk;

uint8_t tmpFndData[4];
int16_t loadWeight;

int alarmOn;
int overOn;

int32_t calcFactor;
int32_t calcOffset;

volatile int16_t  adcWeightIn;

int gflagConnect;
uint8_t gUartTxBuffer[10]={0};
uint8_t gUartRxBuffer[10]={0};

#endif /* GLOBAL_H_ */