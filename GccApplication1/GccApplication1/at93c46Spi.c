/*
 * at93c46Spi.c
 *
 * Created: 6/29/2020 8:37:04 PM
* Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 

#include "header.h"
#include "extern.h"

#define DELAY_SPI	10
void at93c46_ewen(void)
{
	cbi(PORTB,CS);
	SPDR = 0x02;
	_delay_ms(DELAY_SPI);
	SPDR = 0x60;
	_delay_ms(DELAY_SPI);
	sbi(PORTB,CS);
}

void at93c46_ewds(void)
{
	cbi(PORTB,CS);
	SPDR = 0x02;
	_delay_ms(DELAY_SPI);
	SPDR = 0x60;
	_delay_ms(DELAY_SPI);
	sbi(PORTB,CS);
}
uint8_t at93c46_read(uint8_t addr)
{
	uint8_t high, low;
	
	cbi(PORTB, CS);
	SPDR = 0x03;
	_delay_ms(DELAY_SPI);
	SPDR = addr;
	_delay_ms(DELAY_SPI);
	SPDR = 0x00;
	_delay_ms(DELAY_SPI);
	high = SPDR;
	SPDR = 0x00;
	_delay_ms(DELAY_SPI);
	low = SPDR;
	sbi(PORTB, CS);
	return (high << 1) + (low >> 7);
}

void at93c46_write(uint8_t addr, uint8_t dat)
{
	cbi(PORTB, CS);
	SPDR = 0x02;
	_delay_ms(DELAY_SPI);
	SPDR = addr | 0x80;
	_delay_ms(DELAY_SPI);
	SPDR = dat;
	_delay_ms(DELAY_SPI);
	sbi(PORTB, CS);
	_delay_ms(3);
}

int16_t saveCode(int codeNo,int16_t codeData)
{
	uint8_t tmpSPI1,tmpSPI2;
	uint8_t tmpData1,tmpData2;

	switch(codeNo){
		case 0: codePoint		= codeData; break;
		case 1: codeFilt		= codeData; break;
		case 2: codeDivision	= codeData; break;
		case 3: codeAutoZero	= codeData; break;
		case 4: codeAlarm		= codeData; break;
		case 5: codeOver		= codeData; break;
		case 6: codeWeight		= codeData; break;
		case 8: codeAdcSpan		= codeData; break;
		case 9: codeAdcZero		= codeData; break;
	}
	
//	initCodeData();
	tmpData1 = (uint8_t)(codeData >> 8 );
	tmpData2 = (uint8_t)(codeData % 256);
	
	at93c46_ewen();	
	at93c46_write(codeNo * 2, tmpData1);
	at93c46_write(codeNo * 2+1, tmpData2);
	at93c46_ewds();
		
	tmpSPI1 = at93c46_read(codeNo * 2    );
	tmpSPI2 = at93c46_read(codeNo * 2 +1 );
	return ( tmpSPI1 * 256 + tmpSPI2 );	
}

int16_t readCode(int codeNo)
{
	uint8_t tmpSPI1,tmpSPI2;

	tmpSPI1 = at93c46_read(codeNo * 2    );
	tmpSPI2 = at93c46_read(codeNo * 2 +1 );
	return ( tmpSPI1 * 256 + tmpSPI2 );	
}

int readRomData(void)
{	

	codePoint = readCode(0);
	if( (codePoint < 0 ) || (codePoint > 2)) return ERR_CODE_0;
	
	codeFilt		= readCode(1);
	if( (codeFilt < 0 ) || (codeFilt > 9)) return ERR_CODE_1;

	codeDivision	= readCode(2);
	if( (codeDivision < 0 ) || (codeDivision > 5)) return ERR_CODE_2;

	codeAutoZero	= readCode(3);
	if( (codeAutoZero < 0 ) || (codeAutoZero > 1)) return ERR_CODE_3;

	codeAlarm		= readCode(4);
	if( (codeAlarm < 0 ) || (codeAlarm > 9999 )) return ERR_CODE_4;

	codeOver		= readCode(5);
	if( (codeOver < 0 ) || (codeOver > 9999)) return ERR_CODE_5;

	codeWeight		= readCode(6);
	if( (codeWeight < 0 ) || (codeWeight > 9999)) return ERR_CODE_6;

	codeAdcSpan	= readCode(8);
	if( (codeAdcSpan < 0 ) || (codeAdcSpan > 9999)) return ERR_CODE_8;

	codeAdcZero	= readCode(9);
	if( (codeAdcZero < 0 ) || (codeAdcZero > 9999)) return ERR_CODE_9;
	
	return -1;
}

int initRomData(void)
{	
	codePoint = saveCode(0,0);
	if( (codePoint < 0 ) || (codePoint > 2)) return ERR_CODE_0;
	
	codeFilt		= saveCode(1,0);
	if( (codeFilt < 0 ) || (codeFilt > 9)) return ERR_CODE_1;

	codeDivision	= saveCode(2,1);
	if( (codeDivision < 0 ) || (codeDivision > 5)) return ERR_CODE_2;

	codeAutoZero	= saveCode(3,0);
	if( (codeAutoZero < 0 ) || (codeAutoZero > 1)) return ERR_CODE_3;

	codeAlarm		= saveCode(4,1000);
	if( (codeAlarm < 0 ) || (codeAlarm > 9999 )) return ERR_CODE_4;

	codeOver		= saveCode(5,1200);
	if( (codeOver < 0 ) || (codeOver > 9999)) return ERR_CODE_5;

	codeWeight		= saveCode(6,2000);
	if( (codeWeight < 0 ) || (codeWeight > 9999)) return ERR_CODE_6;

	codeAdcSpan	= saveCode(8,750);
	if( (codeAdcSpan < 200 ) || (codeAdcSpan > 1020)) return ERR_CODE_8;

	codeAdcZero	= saveCode(9,10);
	if( (codeAdcZero < 0 ) || (codeAdcZero > 200)) return ERR_CODE_9;
	
	return -1;
}
	 
// end of at93c46Spi.c