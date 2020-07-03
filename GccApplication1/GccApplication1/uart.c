/*
 * uart.c
 *
 * Created: 6/29/2020 11:20:09 PM
 * Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 
#include "header.h"
#include "extern.h"

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define RS485_RX_EN cbi(PORTA,7)
#define RS485_TX_EN sbi(PORTA,7)

#define RESET		0
#define SET			1
#define ID_ADDR		'9'

void initUart()
{
	//uint_fast16_t UBRR_VALUE;
	//UBRR_VALUE = (((F_CPU / (USART_BAUDRATE * 16))) - 1);

	//3. UBRR0은 16비트 레지스터이기 때문에 8비트씩 나누어서 넣어야 한다.
	UBRR1H = (uint8_t)(UBRR_VALUE >>8);
	UBRR1L = (uint8_t) UBRR_VALUE;
	
	//4. USART 설정
	UCSR1C |= (1<<UCSZ10)|(1<<UCSZ11); //Charecter size : 8비트
	UCSR1C &= ~(1<<USBS1); //stop  bit : 1비트
	UCSR1C &= ~((1<<UPM11)|(1<<UPM10)); // no parity mode
	
	//5. 송수신을 가능하게 한다.
	// rx irq enable
	UCSR1B=( 1 << RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
}

void transmitByte(uint8_t data) {
	
	while(!(UCSR1A&(1<<UDRE1))){};	
	UDR1 = data;                                            /* send data */
}

uint8_t receiveByte(void) {
	// 수신 되기를 기다림
	while(!(UCSR1A&(1<<RXC1))){};
	return UDR1;                                /* return register value */
}


void sendString(const char str[]) {
	uint8_t i = 0;
	while (str[i]) {
		transmitByte(str[i]);
		i++;
	}
}

void readString(char str[], uint8_t maxLength) {
	char response;
	uint8_t i;
	i = 0;
	while (i < (maxLength - 1)) {                   /* prevent over-runs */
		response = receiveByte();
		transmitByte(response);                                    /* echo */
		if (response == '\r') {                     /* enter marks the end */
			break;
		}
		else {
			str[i] = response;                       /* add in a letter */
			i++;
		}
	}
	str[i] = 0;                          /* terminal NULL character */
}


ISR(USART1_RX_vect)
{
	static int i;
	unsigned char inData;
	static unsigned char gUartIndex=0;
	static unsigned char flagStx = RESET;
	
	inData = UDR1;
	if(flagStx == RESET){
		if(inData == 0x02){
			gUartIndex = 0;
			gUartRxBuffer[gUartIndex++] = inData;
			flagStx = SET;
		}
	}
	else if(flagStx == SET){
		gUartRxBuffer[gUartIndex++] = inData;
		if(inData == 0x03){

/*
			flagLed = ( flagLed ) ? 0 : 1;\
			if( flagLed ){ LED_ON; }
			else {			LED_OFF;}
*/
			gflagConnect = SET;
			flagStx = RESET;
			for( i = 0 ; i < 6 ; i++) gUartTxBuffer[i] = gUartRxBuffer[i];
			gUartIndex = 0;
		}
	}
}

void toAscii(int16_t indata,uint8_t point, char * returnVal)
{
	int16_t	tmpData;
	char * retVal;
	
	retVal = returnVal;
	
	if(indata < 0)
	{
		tmpData = -indata;
		if(tmpData > 999 ){
			retVal[0] = '-';
			retVal[1] = '9';
			retVal[2] = '9';
			retVal[3] = '9';
			retVal[4] = '.';
		} else {
			retVal[0] = '-';
			retVal[1] = ((unsigned char)(tmpData / 100))			+ 0x30;
			retVal[2] = ((unsigned char)((tmpData % 100 ) /10))	+ 0x30;
			retVal[3] = (unsigned char) ( tmpData %10  )         + 0x30;
			retVal[4] = '.';
		}
	} else {
		if(indata > 9999 ){
			retVal[0] = '9';
			retVal[1] = '9';
			retVal[2] = '9';
			retVal[3] = '9';
			retVal[4] = '.';
			} else {
			retVal[0] = ((unsigned char)(indata / 1000))		+ 0x30;
			retVal[1] = ((unsigned char)((indata %1000)/100))	+ 0x30;
			retVal[2] = ((unsigned char)((indata % 100 )/10))	+ 0x30;
			retVal[3] = (unsigned char) ( indata % 10 )	+ 0x30;
			retVal[4] = '.';
		}
		
	}	
	switch(point){
		case 1:
			retVal[4] = retVal[3];
			retVal[3] = '.';
			break;
		case 2:
			retVal[4] = retVal[3];
			retVal[3] = retVal[2];
			retVal[2] = '.';
			break;
	}
}

void rs485Send( )
{

	int i;
	char retVal[10]={0};

	RS485_TX_EN;

	gUartTxBuffer[0] = 0x02;
	gUartTxBuffer[1] = ID_ADDR;
	gUartTxBuffer[2] = 'A';
	
	toAscii(loadWeight,codePoint, retVal);
	gUartTxBuffer[3] = retVal[0];
	gUartTxBuffer[4] = retVal[1];
	gUartTxBuffer[5] = retVal[2];
	gUartTxBuffer[6] = retVal[3];
	gUartTxBuffer[7] = retVal[4];
	gUartTxBuffer[8] = 0x03;

	_delay_ms(7);
	for( i = 0 ; i < 9 ; i++){
		transmitByte( gUartTxBuffer[i]);
	}
	_delay_ms(10);
	RS485_RX_EN;	
}

void sendToLargeFnd( )
{

	int i;
	char retVal[10]={0};

	RS485_TX_EN;

	gUartTxBuffer[0] = 0xAA;
	gUartTxBuffer[1] = 0x09;	

	// toAscii(loadWeight,codePoint, retVal);
	if( loadWeight < 0){
		gUartTxBuffer[2] = retVal[0];
		gUartTxBuffer[3] = retVal[1];
		gUartTxBuffer[4] = retVal[2];
		gUartTxBuffer[5] = retVal[3];
		gUartTxBuffer[7] = 0xbb;
	} else {
		gUartTxBuffer[2] = loadWeight/1000;
		gUartTxBuffer[3] = (loadWeight%1000)/100;
		gUartTxBuffer[4] = (loadWeight%100)/10;
		gUartTxBuffer[5] = loadWeight%10;
		gUartTxBuffer[7] = 0xbb;
	}
	_delay_ms(7);
	for( i = 0 ; i < 8 ; i++){
		transmitByte( gUartTxBuffer[i]);
	}
	_delay_ms(10);
	RS485_RX_EN;
}

void sciCommandProc()
{
	static uint32_t startCount=0;

	
	if( elaspMsecTime(startCount) > 500 ){
		startCount = timerCounter2;
		sendToLargeFnd( );
	}
	
/*
	RS485_RX_EN;
	if( gflagConnect ){
		gflagConnect = 0;
		if(( gUartTxBuffer[2] == 'E') && (gUartTxBuffer[1] == ID_ADDR)){
			rs485Send( );
		}
	}
*/
}

// end of uart.c