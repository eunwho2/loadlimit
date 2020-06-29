/*
 * fnd.c
 *
 * Created: 6/26/2020 11:10:03 PM
 *  Author: Administrator
 */ 
#include "header.h"
#include "extern.h"

void FND_init(void)
{
	FND_DATA_DDR = 0xff;   // FND Data Port
	FND_CONTROL_DDR |= 0x0f;   // FND Control Port
}

void displayFND(uint8_t position, uint8_t number)
{
	FND_CONTROL_PORT = FND_CONTROL_PORT & 0xf0;
	FND_CONTROL_PORT |= fndTableDigitSelect[position];
	FND_DATA_PORT = fndTableNum[number];
}

void displayNumber(int count)
{
	displayFND(0, count%10);  // 1의 자리
	_delay_ms(1);
	displayFND(1, count%100/10);  // 10의 자리
	_delay_ms(1);
	displayFND(2, count%1000/100);  // 100의 자리
	_delay_ms(1);
	displayFND(3, count/1000);  // 1000의 자리
	_delay_ms(1);
}