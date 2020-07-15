/*
 * GccApplication1.c
 *
 * Created: 6/26/2020 10:54:41 AM
 * Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
*/ 

// global 
#include "header.h"
#include "extern.h"
#include "global.h"

uint32_t elaspMsecTime(uint32_t startTimerCount)
{
	uint32_t tmpReturn;
	
	tmpReturn = ( timerCounter2 < startTimerCount ) ? (4294967295 - startTimerCount + timerCounter2) : timerCounter2 - startTimerCount ;
	return tmpReturn;
}

ISR(TIMER2_OVF_vect)
//void test(void)
{
	static uint8_t fndDigitCount;
	static uint16_t blinkCount;		
	static uint8_t blinkFlagState;
	uint8_t tmpFnd;

	timerCounter2 ++;

//--- fnd out proc
	blinkCount = (blinkCount > 400) ? 0 : blinkCount + 1;
	if(blinkCount == 0 ) { blinkFlagState = ( blinkFlagState ) ? 0 : 1;}

	fndDigitCount = ( fndDigitCount > 2 ) ? 0 : fndDigitCount +1;

	tmpFnd = fndData[fndDigitCount];
	
	if (blinkFlagState){
		if( (blinkCmdFlag & 0x0f) == 0x0f ) tmpFnd = 0x00;
		else if((fndDigitCount == 0) && ((blinkCmdFlag & 0x01) == 0x01 )) tmpFnd = 0x00;
		else if((fndDigitCount == 1) && ((blinkCmdFlag & 0x02) == 0x02 )) tmpFnd = 0x00;
		else if((fndDigitCount == 2) && ((blinkCmdFlag & 0x04) == 0x04 )) tmpFnd = 0x00;
		else if((fndDigitCount == 3) && ((blinkCmdFlag & 0x08) == 0x08 )) tmpFnd = 0x00;
	}

	FND_CONTROL_PORT = FND_CONTROL_PORT & 0xf0;
	FND_CONTROL_PORT |= fndTableDigitSelect[fndDigitCount];
	FND_DATA_PORT = tmpFnd;
//	PORTE |=  LED_ALARM;


//--- end of fnd proc
}

void initADC()
{
	ADMUX = (0x01 << REFS0) ; // channel 0 
	// ADCSRA = (0x01 << ADEN ) |(0x01 << ADSC ) |(0x01<<ADFR)|( 0x01 << ADPS2 )|( 0x01 << ADPS1 )|( 0x01 << ADPS0 );
//	ADCSRA = 0xff;
	//ADMUX = 0b01011110;		// 1.22V 
	ADCSRA = 0b11111111;
	
}
uint16_t testjk;

ISR(ADC_vect)
{
	static int adcRingCount = 0;
	static int16_t adcRingBuf[16]={0};
	int16_t tmpAdc;
	uint8_t tmp;	
	
	tmp = ADCL;
	
	tmpAdc = ( ADCH & 0x03 ) * 256 + tmp;
	testjk = tmpAdc;

	adcWeight -= adcRingBuf[adcRingCount];
	adcRingBuf[adcRingCount] = adcWeightIn;	

	adcWeight += tmpAdc;
	adcRingCount = ( adcRingCount > 14 ) ? 0 : adcRingCount +1 ; 

	adcWeightIn = adcWeight / 16;
	
	ADCSRA |= 0x40;		// start conversion
}

uint8_t getButton(void){
	uint8_t button;
	if( (PINB | 0x0f ) == BTN_NULL ) return BTN_NULL;

	_delay_ms(20);
	if( (PINB | 0x0f ) == BTN_NULL ) return BTN_NULL;
	else {
		button = PINB | 0x0f;
		_delay_ms(100);
		return button;
	}	
}

uint8_t getCommand(void)
{
	return getButton();
}

uint8_t tmpDigit[4];

void displayWeight(int16_t weightIn)
{
	int i;
	int16_t tmpWeight;
	
	// display loadWeight
	if(weightIn < 0 ){
		tmpWeight = -weightIn;
		if(tmpWeight > 998 ){ 
			tmpDigit[2] = 9;
			tmpDigit[1] = 9;
			tmpDigit[0] = 9;
		} else {
			tmpDigit[2] = tmpWeight/100;
			tmpDigit[1] = (tmpWeight%100)/10;
			tmpDigit[0] = tmpWeight%10;
		}
		fndData[3] = 0x40;
		for(i = 0 ; i < 3 ; i++ ) fndData[i] = fndTableNum[tmpDigit[i]];
		if( codePoint != 0 ) fndData[codePoint] |= 0x80;
		return;
	}

	if(weightIn > 9999 ){
		tmpDigit[3] = 9;
		tmpDigit[2] = 9;
		tmpDigit[1] = 9;
		tmpDigit[0] = 9;		
	} else {
		tmpDigit[3] = weightIn/ 1000;
		tmpDigit[2] = (weightIn%1000)/100;
		tmpDigit[1] = (weightIn%100)/10;
		tmpDigit[0] = weightIn%10;
	}
	
	for(i = 0 ; i < 4 ; i++ ) tmpDigit[i] = fndTableNum[tmpDigit[i]];
	
	if(codePoint <= 0 ){
		if(tmpDigit[3] == fndTableNum[0] ) {
			tmpDigit[3] = 0x00;
			if(tmpDigit[2] == fndTableNum[0] ) {
				tmpDigit[2] = 0x00;
				if(tmpDigit[1] == fndTableNum[0] ) {
					tmpDigit[1] = 0x00;
				}
			}
		}
	}
	else if(codePoint == 1 ){
		tmpDigit[1] |= 0x80;
		if(tmpDigit[3] == fndTableNum[0] ) {
			tmpDigit[3] = 0x00;
			if(tmpDigit[2] == fndTableNum[0] )
				tmpDigit[2] = 0x00;
		}
	}
	else if(codePoint == 2 ){
		tmpDigit[2] |= 0x80;
		if(tmpDigit[3] == fndTableNum[0] ) {
			tmpDigit[3] = 0x00;
		}
	}

	for(i = 0 ; i < 4 ; i++ ) fndData[i] = tmpDigit[i];
}

void displayWeight1(int16_t weightIn)
{
	int i;
	int16_t tmpWeight;
	
	// display loadWeight
	if(weightIn < 0 ){
		tmpWeight = -weightIn;
		if(tmpWeight > 998 ){
			tmpDigit[3] = 0x40;
			tmpDigit[2] = fndTableNum[9];
			tmpDigit[1] = fndTableNum[9];
			tmpDigit[0] = fndTableNum[9];
			
			} else {
			tmpDigit[3] = 0x40;
			tmpDigit[2] = tmpWeight/100;
			tmpDigit[1] = (tmpWeight%100)/10;
			tmpDigit[0] = tmpWeight%10;
			for(i = 0 ; i < 3 ; i++ ) fndData[i] = fndTableNum[tmpDigit[i]];
		}
		if( codePoint != 0 ) fndData[codePoint] |= 0x80;
		return;
	}

	if(weightIn > 9999 ){
		tmpDigit[3] = 9;
		tmpDigit[2] = 9;
		tmpDigit[1] = 9;
		tmpDigit[0] = 9;
		} else {
		tmpDigit[3] = weightIn/ 1000;
		tmpDigit[2] = (weightIn%1000)/100;
		tmpDigit[1] = (weightIn%100)/10;
		tmpDigit[0] = weightIn%10;
	}
	
	for(i = 0 ; i < 4 ; i++ ) tmpDigit[i] = fndTableNum[tmpDigit[i]];
	
	if(codePoint == 1 )			tmpDigit[1] |= 0x80;
	else if(codePoint == 2 )	tmpDigit[2] |= 0x80;

	for(i = 0 ; i < 4 ; i++ ) fndData[i] = tmpDigit[i];
}

void displayNumber1(int16_t weightIn)
{
	int i;
	int16_t tmpWeight;
	
	// display loadWeight
	if(weightIn < 0 ){
		tmpWeight = -weightIn;
		if(tmpWeight > 998 ){
			tmpDigit[3] = 0x40;
			tmpDigit[2] = fndTableNum[9];
			tmpDigit[1] = fndTableNum[9];
			tmpDigit[0] = fndTableNum[9];
			
			} else {
			tmpDigit[3] = 0x40;
			tmpDigit[2] = tmpWeight/100;
			tmpDigit[1] = (tmpWeight%100)/10;
			tmpDigit[0] = tmpWeight%10;
			for(i = 0 ; i < 3 ; i++ ) fndData[i] = fndTableNum[tmpDigit[i]];
		}
		if( codePoint != 0 ) fndData[codePoint] |= 0x80;
		return;
	}

	if(weightIn > 9999 ){
		tmpDigit[3] = 9;
		tmpDigit[2] = 9;
		tmpDigit[1] = 9;
		tmpDigit[0] = 9;
		} else {
		tmpDigit[3] = weightIn/ 1000;
		tmpDigit[2] = (weightIn%1000)/100;
		tmpDigit[1] = (weightIn%100)/10;
		tmpDigit[0] = weightIn%10;
	}
	
	for(i = 0 ; i < 4 ; i++ ) tmpDigit[i] = fndTableNum[tmpDigit[i]];
	
	if(tmpDigit[3] == fndTableNum[0] ) {
		tmpDigit[3] = 0x00;
		if(tmpDigit[2] == fndTableNum[0] ) {
			tmpDigit[2] = 0x00;
			if(tmpDigit[1] == fndTableNum[0] ) {
				tmpDigit[1] = 0x00;
			}
		}
	}

	for(i = 0 ; i < 4 ; i++ ) fndData[i] = tmpDigit[i];
}

void displayNumber(int16_t weightIn)
{
	int i;
	int16_t	tmpWeight;
	static uint32_t startmsecCount;
	if( elaspMsecTime(startmsecCount) < 300 ) return;

	startmsecCount = timerCounter2;

	if(weightIn < 0 ){
		tmpWeight = -weightIn;
		if(tmpWeight > 998 ){
			tmpDigit[3] = 0x40;
			tmpDigit[2] = fndTableNum[9];
			tmpDigit[1] = fndTableNum[9];
			tmpDigit[0] = fndTableNum[9];
			
			} else {
			tmpDigit[3] = 0x40;
			tmpDigit[2] = tmpWeight/100;
			tmpDigit[1] = (tmpWeight%100)/10;
			tmpDigit[0] = tmpWeight%10;
			for(i = 0 ; i < 3 ; i++ ) fndData[i] = fndTableNum[tmpDigit[i]];
		}
		return;
	}

	tmpDigit[3] = weightIn/1000;
	tmpDigit[2] = (weightIn%1000)/100;
	tmpDigit[1] = (weightIn%100)/10;
	tmpDigit[0] = weightIn%10;	
	for(i=0;i < 4 ; i++ ) fndData[i] = fndTableNum[tmpDigit[i]];
}

void initGpio()
{
	DDRA		= 0x0f;   // FND Control Port
	DDRB		= 0x07;   // SSI and Button
	DDRC		= 0xff;   // FND Data Port
	DDRD		= 0x08;   // UART and reserved
	DDRE = 0xfe;
	PORTE = 0x7e;

	TIMSK =  0x40;
	TCNT2 = 0x00;
	TCCR2 =  0x03;

	sbi(PORTB,CS);
	SPCR = 0x5D;
	SPSR= 0x00;
	
	sbi(PORTE,LED_ALARM);
	sbi(PORTE,LED_OVER);
	
//	SFIOR = 0xff;
}

void calcWeightCoeff()
{
	calcFactor = (int32_t ) codeWeight * 1024 ;
	calcFactor = calcFactor / ( codeAdcSpan - codeAdcZero );
	calcOffset = (int32_t ) codeWeight * 1024;
	calcOffset = - calcOffset * codeAdcZero / ( codeAdcSpan - codeAdcZero);
}

void initCodeData(){

	calcWeightCoeff();
	alarmOn = 0;
	overOn	= 0;
	gflagConnect = 0;
	tripNumber = 0;
	codeNumber = 0;
	blinkCmdFlag = 0x00;
}

int returnVal;

int main(void)
{
	uint8_t keyBuf;
	
	initGpio();
	
	initADC();	
	
	initUart();
	
	sei();	

    initUart();

	readRomData();
	initCodeData();		
	loadWeight = readLoad();
	enterModeRun();
    while (1) 
    {	
		procRelayOut( );
		sciCommandProc( );	
		command = getCommand();
		if(command != BTN_NULL ) keyBuf = 1;
		loadWeight = readLoad();
		switch( machineState ){
			case MODE_RUN			: modeRun(command)			; break;
			case MODE_PASSWD		: modePassWord(command)		; break;
			case MODE_SELECT_CODE	: modeSelectCode(command)	; break;
			case MODE_CHANGE_CODE	: modeChangeCode(command)	; break;
			case MODE_ERROR			: modeError(command)		; break;			
			default					: enterModeRun( )	; break;
		}
		if(keyBuf ==  1){
			while ( getCommand() != BTN_NULL ) ;
			keyBuf = 0;
		}
    }
}


void procRelayOut(void)
{
	overOn  = ( loadWeight > codeOver  ) ? 1 : 0;
	alarmOn = ( loadWeight > codeAlarm ) ? 1 : 0;
	
//--- alarm 
	if( alarmOn){
		sbi(PORTA,RELAY_ALARM);
		cbi(PORTE,LED_ALARM); 
	} else {
		cbi(PORTA,RELAY_ALARM);
		sbi(PORTE,LED_ALARM);
	}
	
//--- over relay led	
	if( overOn){
		sbi(PORTA,RELAY_OVER);
		cbi(PORTE,LED_OVER);
	} else {
		cbi(PORTA,RELAY_OVER);
		sbi(PORTE,LED_OVER);
	}

// test button 
	if( (PIND & 0x01) == 0){
		// displayWeight(codeOver);
		sbi(PORTA,RELAY_ALARM);
		cbi(PORTE,LED_ALARM);

		sbi(PORTA,RELAY_OVER);
		cbi(PORTE,LED_OVER);
	}
}

void systemErrProc( uint8_t err_no)
{
	
}

int16_t readLoad(void)
{
	int i,j;
	static int16_t weightBuf[10] ={0};
	static int ringCountFilt = 0;
	int16_t weight1;
	int32_t weightSum;
	static uint32_t startmsecCount=0;
	
	if( elaspMsecTime(startmsecCount) < 100 ) return loadWeight ;
	startmsecCount = timerCounter2;

	if(adcWeightIn > 1020 ){
		tripNumber = 3;
		enterModeError();
		return 0;
	} else if ( adcWeightIn < 0 ){
		tripNumber = 4;
		enterModeError();
		return 0;		
	}

	weight1 = (int16_t)(( calcFactor * adcWeightIn  + calcOffset ) / 1024) ;

	// weight1 = (int16_t)( tmp >> 10 );
	
	weightBuf[ringCountFilt] = weight1;
	ringCountFilt = (ringCountFilt > 8 ) ? 0 : ringCountFilt +1;
	
//--- filt proc	
	j = ringCountFilt;
	weightSum = 0;
	for ( i = 0; i < (codeFilt + 1) ; i++ ){
		weightSum += weightBuf[j];
		j = ( j < 1 ) ?  9 : j - 1;
	}	
	weight1 = weightSum / ( codeFilt + 1);	 

//-- divison proc
	if( codeDivision == 0 ) weight1 = ( ( weight1 + 4)  / 10 ) * 10;
	else weight1 = ( weight1  / codeDivision ) * codeDivision;

	return weight1;	
}

void checkRomData(void)
{
	int returnVal;
	
	returnVal = readRomData();
	if(returnVal != -1 ){
		codeNumber = 10;
		enterModePassWord();
		do{
			command = getCommand();
			returnVal = modePassWord(command);
		} while ( returnVal == 0 );
		if( returnVal == -1 ) initRomData();
	}
}