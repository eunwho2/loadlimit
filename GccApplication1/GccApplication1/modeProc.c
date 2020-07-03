/*
 * modeProc.c
 *
 * Created: 6/27/2020 8:19:39 PM
* Author : 정순길 Soon-kil Jung, Dong-Ho Power Electronics
 */ 
#include "header.h"
#include "extern.h"

void enterModeRun(void)
{
	initCodeData();
	machineState = MODE_RUN;
	loadWeight = readLoad();
	displayWeight(loadWeight);
}

void enterModePassWord(void)
{
	int i;
	machineState = MODE_PASSWD;
	for( i = 0 ; i < 4 ; i++){
		passWord[i] = 0;	//
		fndData[i] = fndTableNum[0];
	}
	blinkCmdFlag = 0x08;
	editDigitPoint = 3;		// 1000
}

void enterModeSelectCode(void)
{
	machineState = MODE_SELECT_CODE;
	codeNumber = 0;
	fndData[3] = fndTableChar['P' - 'A'];
	fndData[2] = fndTableChar['O' - 'A'];
	fndData[1] = fndTableChar['I' - 'A'];
	fndData[0] = fndTableChar['T' - 'A'];
	blinkCmdFlag = 0x0f;	//
}

void enterModeChangeCode(void)
{
	machineState = MODE_CHANGE_CODE;

	switch(codeNumber){	
	case 0:
		tmpCodeData = codePoint;
		blinkCmdFlag = 0x00;	// no blink	
		displayWeight(loadWeight);
		break;
	case 1:
		tmpCodeData = codeFilt;	//
		blinkCmdFlag = 0x00;
		fndData[3] = 0x00;
		fndData[2] = 0x00;
		fndData[1] = 0x00;
		fndData[0] = fndTableNum[tmpCodeData];
		break;
	case 2:
		tmpCodeData = codeDivision;	//
		blinkCmdFlag = 0x00;
		fndData[3] = 0x00;
		fndData[2] = 0x00;
		fndData[1] = 0x00;
		fndData[0] = fndTableNum[tmpCodeData];
		break;
	case 3:
		tmpCodeData = codeAutoZero;
		blinkCmdFlag = 0x0f;	//
		if(tmpCodeData){
			fndData[3] = 0x00;
			fndData[2] = fndTableChar['Y' - 'A'];
			fndData[1] = fndTableChar['E' - 'A'];
			fndData[0] = fndTableChar['S' - 'A'];
		} else {
			fndData[3] = 0x00;
			fndData[2] = fndTableChar['N' - 'A'];
			fndData[1] = fndTableChar['O' - 'A'];
			fndData[0] = 0x00;
		}
		break;
	case 4:
		tmpCodeData = codeAlarm;
		editDigitPoint = 0;		// 1000
		blinkCmdFlag = 0x01;
		displayWeight(tmpCodeData);
		break;
	case 5:
		tmpCodeData = codeOver;
		editDigitPoint = 0;		// 1000
		blinkCmdFlag = 0x01;
		displayWeight(tmpCodeData);
		break;
	case 6:
		tmpCodeData = codeWeight;
		editDigitPoint = 0;		// 1000
		blinkCmdFlag = 0x01;
		displayWeight(tmpCodeData);
		break;
	case 7:
		editDigitPoint = 0;		// 1000
		blinkCmdFlag = 0x00;
		displayNumber(adcWeightIn);
		break;
	default:
		systemErrProc(ERR_SYS_NO_CODE_NUMBER);
		break;		
	}
}

int modeRun(uint8_t cmd)
{
	if( cmd  == BTN_ENTER ){ 
		enterModePassWord();
		//enterModeSelectCode();
		return 0;
	}
	displayWeight(loadWeight);
	return 0;
}


int modePassWord(uint8_t cmd )
{
	int i;
	int16_t passwd;
	
	if( cmd == BTN_ENTER ){
		passwd = passWord[3] * 1000 +passWord[2] * 100 + passWord[1] * 10 + passWord[0]; 	
		if( passwd == 3262 ){
			if(codeNumber == 6 ){
				saveCode(6,tmpCodeData);
				saveCode(8,adcWeight / 16);
				enterModeRun();				
			} else if ( codeNumber == 10) {
				return -1;				
			} else {
				enterModeSelectCode();
			}
		} else {
			if(codeNumber == 10 ) return -2;
			else  enterModeRun();
		}
		return 0;
	}

	if(cmd == BTN_CANCEL){
		if(codeNumber == 10 ) return -2;
		else			enterModeRun();
		return 0;
	}
	
	if(cmd == BTN_UP){
		passWord[editDigitPoint] = ( passWord[editDigitPoint] > 8 ) ? 0 : passWord[editDigitPoint] +1;
		i = passWord[editDigitPoint];
		fndData[editDigitPoint] = fndTableNum[i];
		return 0;
	}

	if(cmd == BTN_LEFT){
		editDigitPoint = ( editDigitPoint <= 0 ) ? 3 : editDigitPoint - 1;
		blinkCmdFlag = blinkCmdFlag >> 1 ;
		if( blinkCmdFlag == 0 ) blinkCmdFlag = 0x08;
		
		i = passWord[editDigitPoint];
		fndData[editDigitPoint] = fndTableNum[i];
		return 0;
	}
	return 0;
}

int modeSelectCode(uint8_t cmd)
{

	if( cmd == BTN_ENTER ){
		enterModeChangeCode();
		return 0;
	}

	if(cmd == BTN_CANCEL){
		enterModeRun();
		return 0;
	}
	
	if(cmd == BTN_UP){
		codeNumber = ( codeNumber > 7 ) ? 0 : codeNumber +1;
		switch(codeNumber){
			case 0 :
				fndData[3] = fndTableChar['P' - 'A'];
				fndData[2] = fndTableChar['O' - 'A'];
				fndData[1] = fndTableChar['I' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case 1 :
				fndData[3] = fndTableChar['F' - 'A'];
				fndData[2] = fndTableChar['I' - 'A'];
				fndData[1] = fndTableChar['L' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case 2 :
				fndData[3] = fndTableChar['D' - 'A'];
				fndData[2] = fndTableChar['I' - 'A'];
				fndData[1] = fndTableChar['V' - 'A'];
				fndData[0] = fndTableChar['I' - 'A'];
				break;
			case 3 :
				fndData[3] = fndTableChar['A' - 'A'];
				fndData[2] = fndTableChar['U' - 'A'];
				fndData[1] = fndTableChar['T' - 'A'];
				fndData[0] = fndTableChar['O' - 'A'];
				break;
			case 4 :
				fndData[3] = fndTableChar['A' - 'A'];
				fndData[2] = fndTableChar['L' - 'A'];
				fndData[1] = fndTableChar['A' - 'A'];
				fndData[0] = fndTableChar['R' - 'A'];
				break;
			case 5 :
				fndData[3] = fndTableChar['O' - 'A'];
				fndData[2] = fndTableChar['V' - 'A'];
				fndData[1] = fndTableChar['E' - 'A'];
				fndData[0] = fndTableChar['R' - 'A'];
				break;
			case 6 :
				fndData[3] = fndTableChar['S' - 'A'];
				fndData[2] = fndTableChar['E' - 'A'];
				fndData[1] = fndTableChar['T' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case 7 :
				fndData[3] = 0x40;
				fndData[2] = fndTableChar['A' - 'A'];
				fndData[1] = fndTableChar['D' - 'A'];
				fndData[0] = 0x40;
				break;
			default:	
				enterModeSelectCode();			
				break;
		}
		return 0;
	}

	if(cmd == BTN_LEFT){
		return 0;
	}
	return 0;		
}

int modeChangeCode(uint8_t cmd)
{
	int16_t tmp;
	
	if(cmd == BTN_ENTER){
		if( codeNumber == 6 ) {
			tmp = tmpCodeData - codeAdcZero;
			if(tmp < 0 ) tmp = - tmp;
			if( tmp < 20 ){
				tripNumber = 1;
				enterModeError();
			} else if (( adcWeight / 16 ) > 1000 ){
				tripNumber = 2;
				enterModeError();				
			}
			else  enterModePassWord();
		} else if(codeNumber == 3){
			saveCode(codeNumber,tmpCodeData);
			saveCode(9,adcWeight / 16);
			enterModeRun();			
		} else {
			saveCode(codeNumber,tmpCodeData);
			enterModeRun();
		}
		return 0;
	}
	
	if(cmd == BTN_CANCEL){
		enterModeRun();
		return 0;
	}

	if(codeNumber == 7)	{
		displayNumber(adcWeightIn);
		return 0;
	}

	if(cmd == BTN_UP){
		switch(codeNumber){
		case 0:
			tmpCodeData = ( tmpCodeData > 1 ) ? 0 : tmpCodeData + 1;
			codePoint = tmpCodeData;
			displayWeight(loadWeight);
			//displayNumber(loadWeight);
			break;
		case 1:
			tmpCodeData = ( tmpCodeData > 8 ) ? 0 : tmpCodeData + 1;
			//displayWeight(tmpCodeData);
			displayNumber(loadWeight);
			break;
		case 2:
			switch(tmpCodeData){
				case 0: tmpCodeData = 1; break;
				case 1: tmpCodeData = 2; break;
				case 2: tmpCodeData = 5; break;
				case 5: tmpCodeData = 0; break;
				default: tmpCodeData = 1; break;
			}
			displayNumber(tmpCodeData);
			break;
		case 3:
			if(tmpCodeData){
				tmpCodeData = 0;
				fndData[3] = 0x00;
				fndData[2] = fndTableChar['N' - 'A'];
				fndData[1] = fndTableChar['O' - 'A'];
				fndData[0] = 0x00;
			} else {
				tmpCodeData = 1;
				fndData[3] = 0x00;
				fndData[2] = fndTableChar['Y' - 'A'];
				fndData[1] = fndTableChar['E' - 'A'];
				fndData[0] = fndTableChar['S' - 'A'];
			}
			break;
		case 4:
		case 5:
		case 6:
			tmpFndData[editDigitPoint] = ( tmpFndData[editDigitPoint] > 8 ) ? 0 : tmpFndData[editDigitPoint] +1;
			tmpCodeData = tmpFndData[3] * 1000 +tmpFndData[2]*100 + tmpFndData[1] * 10 + tmpFndData[0];
			//displayWeight(tmpCodeData);
			displayNumber(loadWeight);
			break;
		}
	}

	if(cmd == BTN_LEFT){
		switch(codeNumber){
		case 0:
			tmpCodeData = ( tmpCodeData <= 0 ) ? 2 : tmpCodeData - 1;
			codePoint = tmpCodeData;
			displayWeight(loadWeight);
			break;
		case 4:
		case 5:
		case 6:
			editDigitPoint = ( editDigitPoint <= 0 ) ? 3 : editDigitPoint - 1;
			switch(editDigitPoint){
				case 0 : blinkCmdFlag = 0x01; break;
				case 1 : blinkCmdFlag = 0x02; break;
				case 2 : blinkCmdFlag = 0x04; break;
				case 3 : blinkCmdFlag = 0x08; break;
			}	
			//displayWeight(tmpCodeData);
			displayNumber(loadWeight);
			break;
		}
	}
	return 0;
}

void enterModeError(void)
{
	machineState = MODE_ERROR;
	fndData[3] = fndTableChar['E' - 'A'];
	fndData[2] = 0x40;
	fndData[1] = fndTableNum[0];
	fndData[0] = fndTableNum[tripNumber];
	blinkCmdFlag = 0x0f;	//
}

int modeError(uint8_t cmd)
{
	if(cmd == BTN_ENTER){
		enterModeRun();
		return 0;
	}
	return 0;
}
