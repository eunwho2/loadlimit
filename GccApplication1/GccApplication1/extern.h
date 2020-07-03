/*
 * extern.h
 *
 * Created: 6/28/2020 8:01:30 AM
* Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 


#ifndef EXTERN_H_
#define EXTERN_H_

extern const uint8_t fndTableChar[26];
extern const uint8_t fndTableNum[10];
extern const uint8_t fndTableDigitSelect[4];

extern int16_t codePoint;
extern int16_t codeFilt;
extern int16_t codeDivision;
extern int16_t codeAutoZero;
extern int16_t codeAlarm;
extern int16_t codeOver;
extern int16_t codeWeight;
extern int16_t codeAdcSpan;
extern int16_t codeAdcZero;

extern int tripNumber;

extern volatile int x;

extern volatile uint32_t timerCounter2;
extern volatile unsigned int FndValue;

extern volatile uint8_t machineState;
extern uint8_t blinkCmdFlag; // bit 0 : digit 1 blinking    bit 1 : digit 10 blinking
extern uint8_t fndData[4];

extern uint8_t command;

extern uint8_t passWord[4];
extern uint8_t  editDigitPoint;		// digit for change
extern int16_t adcWeight;
extern int codeNumber;
extern int16_t tmpCodeData;
extern uint8_t tmpFndData[4];
extern int16_t loadWeight;

extern int alarmOn;
extern int overOn;

extern int32_t calcFactor;
extern int32_t calcOffset;

extern volatile int16_t  adcWeightIn;

extern int gflagConnect;
extern uint8_t gUartTxBuffer[10];
extern uint8_t gUartRxBuffer[10];

#endif /* EXTERN_H_ */