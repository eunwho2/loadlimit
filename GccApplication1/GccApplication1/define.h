/*
 * define.h
 *
 * Created: 6/28/2020 8:06:38 AM
 * Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 
#ifndef DEFINE_H_
#define DEFINE_H_

#define F_CPU 16000000UL

#define LED_ALARM	6
#define LED_OVER	7

// pcb name error 2020-0702
//#define RELAY_ALARM	5
// #define RELAY_OVER	6
#define RELAY_ALARM	6
#define RELAY_OVER	5

#define BTN_NULL	0xff
#define BTN_UP		0xef
#define BTN_LEFT	0xdf
#define BTN_CANCEL	0xbf
#define BTN_ENTER	0x7f


#define MODE_RUN			0
#define MODE_PASSWD			1
#define MODE_SELECT_CODE	2
#define MODE_CHANGE_CODE	3
#define MODE_ERROR			4

#define FND_DATA_DDR		DDRC   // FND Data Port
#define FND_CONTROL_DDR		DDRA   // FND Control Port
#define FND_DATA_PORT		PORTC   // FND Data Port
#define FND_CONTROL_PORT	PORTA   // FND Control Port

#define sbi(port,bit)	(port) |= (1<<(bit))
#define cbi(port,bit)	(port) &= ~(1<<(bit))
#define CS 0

// define error 
#define ERR_SCALE_TOO_LOW		1		// gain and zero differ below 20
#define ERR_SCALE_TOO_HIGH		2		// gain adc over 1000
#define ERR_ADC_OVER			3		// adc value over 1020
#define ERR_ADC_UNDER			4		// adc below 0

#define ERR_SYS_NO_CODE_NUMBER	50		// no codeNumber

#define ERR_CODE_0				70
#define ERR_CODE_1				71
#define ERR_CODE_2				72
#define ERR_CODE_3				73
#define ERR_CODE_4				74
#define ERR_CODE_5				75
#define ERR_CODE_6				76
#define ERR_CODE_7				77
#define ERR_CODE_8				78
#define ERR_CODE_9				79


#endif /* DEFINE_H_ */