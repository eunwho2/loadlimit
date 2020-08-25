/*
 * module.h
 *
 * Created: 6/28/2020 8:00:40 AM
 * Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 


#ifndef MODULE_H_
#define MODULE_H_

// modeProc.c
int16_t saveCode(int codeNo,int16_t codeData);
void initADC();

void enterModeRun(void);
void enterModePassWord(void);
void enterModeSelectCode(void);
void enterModeChangeCode(void);
void enterModeError(int errNo);

int modeRun(uint8_t cmd);
int modePassWord(uint8_t cmd );
int modeSelectCode(uint8_t cmd);
int modeChangeCode(uint8_t cmd);
int modeError(uint8_t cmd);
void displayErr(int errNo);

// main.c

void TimerInterruptInit(void);
uint32_t elaspMsecTime(uint32_t startTimerCount);
uint8_t getButton(void);
uint8_t getCommand(void);
int16_t readLoad(void);
void displayWeight(int16_t weightIn);

void systemErrProc( uint8_t err_no);
void procRelayOut(void );
void initCodeData();
void displayNumber(int16_t weightIn);
void displayNumber1(int16_t weightIn);
void displayWeight1(int16_t weightIn);
void calcWeightCoeff();
void calcWeightOffset();

//--- at93c46
void at93c46_ewen(void);
void at93c46_ewds(void);
uint8_t at93c46_read(uint8_t addr);
void at93c46_write(uint8_t addr, uint8_t dat);
int16_t saveCode(int codeNo,int16_t codeData);
int16_t readCode(int codeNo);
int readRomData(void);
int initRomData(void);

void initUart();
void transmitByte(uint8_t data);
uint8_t receiveByte(void);
void readString(char str[], uint8_t maxLength);
void sciCommandProc();
void toAscii(int16_t indata,uint8_t point, char * returnVal);
void sendToLargeFnd( );

#endif /* MODULE_H_ */