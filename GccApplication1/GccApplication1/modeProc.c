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

void setEditTmpFndData(int16_t arg1)
{		
	tmpFndData[3] = arg1 / 1000;
	tmpFndData[2] = (arg1 % 1000) /100;
	tmpFndData[1] = ( arg1 % 100 ) / 10;
	tmpFndData[0] = arg1 % 10 ;

	editDigitPoint = 0;		
	blinkCmdFlag = 0x01;
	displayWeight1(arg1);
}

void enterModeChangeCode(void)
{
	machineState = MODE_CHANGE_CODE;

	switch(codeNumber){	
	case CODE_POINT:
		tmpCodeData = codePoint;
		if(tmpCodeData == 0) blinkCmdFlag = 1;
		else if(tmpCodeData == 1) blinkCmdFlag = 2;
		else if(tmpCodeData == 2) blinkCmdFlag = 4;
		else {
			tmpCodeData = 0;
			blinkCmdFlag = 1; 
		}		
		displayWeight1(loadWeight);
		break;
	case CODE_FILT:
		tmpCodeData = codeFilt;	//
		blinkCmdFlag = 1;
		displayNumber1(tmpCodeData);		// no point add space 
		break;
	case CODE_DIVISION:
		tmpCodeData = codeDivision;	//
		blinkCmdFlag = 1;
		displayNumber1(tmpCodeData);		// no point add space
		break;
	case CODE_AUTO:
		tmpCodeData = 0;
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
	case CODE_ALARM:
		tmpCodeData = codeAlarm;
		setEditTmpFndData(tmpCodeData);
		break;
	case CODE_OVER:
		tmpCodeData = codeOver;
		setEditTmpFndData(tmpCodeData);
		break;
	case CODE_SETT:
		tmpCodeData = codeWeight;
		setEditTmpFndData(tmpCodeData);
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
		// enterModeSelectCode();
		return 0;
	}

	if( (PIND & 0x01) == 0) displayWeight(codeOver);
	else					displayWeight(loadWeight);
	return 0;
}


int modePassWord(uint8_t cmd )
{
	int i;
	// static int retryPassWord = 0;
	int16_t passwd;
	// uint8_t tmp;

/*
	if( cmd != BTN_NULL ){
		retryPassWord = 0 ;
		for( i = 0 ; i < 4 ; i++){
			tmp = passWord[i];
			fndData[i] = fndTableNum[tmp];
		}
		blinkCmdFlag = 0x08;
		editDigitPoint = 3;		// 1000
		return 0;
	}
*/	
	if( cmd == BTN_ENTER ){
		passwd = passWord[3] * 1000 +passWord[2] * 100 + passWord[1] * 10 + passWord[0]; 	
		if( passwd == 5737 ){
			enterModeSelectCode();
			return 0;
		} else {
			tripNumber = 5;
			enterModeError(tripNumber);
			
			// displayErr(ERR_PASS_WORD);
			// retryPassWord = 1;
			return 0;
		}
		return 0;
	}

	if(cmd == BTN_CANCEL){
		enterModeRun();
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
			case CODE_POINT :
				fndData[3] = fndTableChar['P' - 'A'];
				fndData[2] = fndTableChar['O' - 'A'];
				fndData[1] = fndTableChar['I' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case CODE_FILT :
				fndData[3] = fndTableChar['F' - 'A'];
				fndData[2] = fndTableChar['I' - 'A'];
				fndData[1] = fndTableChar['L' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case CODE_DIVISION :
				fndData[3] = fndTableChar['D' - 'A'];
				fndData[2] = fndTableChar['I' - 'A'];
				fndData[1] = fndTableChar['V' - 'A'];
				fndData[0] = fndTableChar['I' - 'A'];
				break;
			case CODE_AUTO :
				fndData[3] = fndTableChar['A' - 'A'];
				fndData[2] = fndTableChar['U' - 'A'];
				fndData[1] = fndTableChar['T' - 'A'];
				fndData[0] = fndTableChar['O' - 'A'];
				break;
			case CODE_ALARM :
				fndData[3] = fndTableChar['A' - 'A'];
				fndData[2] = fndTableChar['L' - 'A'];
				fndData[1] = fndTableChar['A' - 'A'];
				fndData[0] = fndTableChar['R' - 'A'];
				break;
			case CODE_OVER :
				fndData[3] = fndTableChar['O' - 'A'];
				fndData[2] = fndTableChar['V' - 'A'];
				fndData[1] = fndTableChar['E' - 'A'];
				fndData[0] = fndTableChar['R' - 'A'];
				break;
			case CODE_SETT :
				fndData[3] = fndTableChar['S' - 'A'];
				fndData[2] = fndTableChar['E' - 'A'];
				fndData[1] = fndTableChar['T' - 'A'];
				fndData[0] = fndTableChar['T' - 'A'];
				break;
			case CODE_ADC :
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
		if( codeNumber == CODE_SETT ) {
			tmp = adcWeightIn - codeAdcZero;
			if(tmp < 0 ) tmp = - tmp;
			if( tmp < 20 ){
				tripNumber = 1;
				enterModeError(tripNumber);
			} else if ( adcWeightIn > 1000 ){
				tripNumber = 2;
				enterModeError(tripNumber);				
			} else {
				saveCode(CODE_SETT,tmpCodeData);
				saveCode(CODE_SPAN_ADC,adcWeightIn);
				saveCode(CODE_ZERO_ADC,0);
				calcWeightCoeff();
				enterModeRun();
			}
			return 0;
		} else if(codeNumber == CODE_AUTO){
			if(tmpCodeData == 0 ){
				 enterModeRun();
				 return 0;
			}
			
			tmp = adcWeightIn - codeAdcZero;
			if(tmp < 0 ) tmp = - tmp;
			if( tmp < 20 ){
				tripNumber = 1;
				enterModeError(tripNumber);
			} else {				
				saveCode(CODE_ZERO_ADC,adcWeightIn);
				// calcWeightCoeff();
				enterModeRun();
			}			
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

	if(codeNumber == CODE_ADC )	{
		displayNumber(adcWeightIn);
		return 0;
	}

	if(cmd == BTN_UP){
		switch(codeNumber){
		case CODE_POINT:
			tmpCodeData = ( tmpCodeData > 1 ) ? 0 : tmpCodeData + 1;
			if(tmpCodeData == 0) blinkCmdFlag = 1;
			else if(tmpCodeData == 1) blinkCmdFlag = 2;
			else if(tmpCodeData == 2) blinkCmdFlag = 4;
			else {
				tmpCodeData = 0;
				blinkCmdFlag = 1;
			}
			codePoint = tmpCodeData;
			displayWeight1(loadWeight);
			//displayNumber(loadWeight);
			break;
		case CODE_FILT:
			tmpCodeData = ( tmpCodeData > 8 ) ? 0 : tmpCodeData + 1;
			//displayWeight(tmpCodeData);
			displayNumber1(tmpCodeData);
			break;
		case CODE_DIVISION:
			switch(tmpCodeData){
				case 0: tmpCodeData = 1; break;
				case 1: tmpCodeData = 2; break;
				case 2: tmpCodeData = 5; break;
				case 5: tmpCodeData = 0; break;
				default: tmpCodeData = 1; break;
			}
			displayNumber1(tmpCodeData);
			break;
		case CODE_AUTO:
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
		case CODE_ALARM:
		case CODE_OVER:
		case CODE_SETT:
			tmpFndData[editDigitPoint] = ( tmpFndData[editDigitPoint] > 8 ) ? 0 : tmpFndData[editDigitPoint] +1;
			tmpCodeData = tmpFndData[3] * 1000 +tmpFndData[2]*100 + tmpFndData[1] * 10 + tmpFndData[0];
			displayWeight1(tmpCodeData);
			//displayNumber(tmpCodeData);
			break;
		}
	}

	if(cmd == BTN_LEFT){
		switch(codeNumber){
		case 0:
			tmpCodeData = ( tmpCodeData < 1 ) ? 2 : tmpCodeData - 1;
			if(tmpCodeData == 0) blinkCmdFlag = 1;
			else if(tmpCodeData == 1) blinkCmdFlag = 2;
			else if(tmpCodeData == 2) blinkCmdFlag = 4;
			else {
				tmpCodeData = 0;
				blinkCmdFlag = 1;
			}
			codePoint = tmpCodeData;
			displayWeight1(loadWeight);
			//displayNumber(loadWeight);
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
			displayWeight1(tmpCodeData);
			// displayNumber(tmpCodeData);
			break;
		}
	}
	return 0;
}

void enterModeError(int errNo)
{
	cntModeErr = timerCounter2;
	mStateBk = machineState;
	machineState = MODE_ERROR;
	//codeNumberBk = codeNumber;
	//tmpCodeDataBk = tmpCodeData;
	
	fndData[3] = fndTableChar['E' - 'A'];
	fndData[2] = 0x40;
	fndData[1] = fndTableNum[0];
	fndData[0] = fndTableNum[errNo];
	blinkCmdFlag = 0x0f;			//
}

int modeError(uint8_t cmd)
{
	int i;
	uint8_t tmp;
	
	if( elaspMsecTime(cntModeErr) > 3000 ) {
		machineState = mStateBk;		
		switch(mStateBk){
		case MODE_RUN:
			enterModeRun();
			break;
		case MODE_PASSWD:
			machineState = MODE_PASSWD;
			for( i = 0 ; i < 4 ; i++){
				tmp = passWord[i];
				fndData[i] = fndTableNum[tmp];
			}
			blinkCmdFlag = 0x08;
			editDigitPoint = 3;		// 1000
			break;
		case MODE_CHANGE_CODE	:
			
			if(codeNumber == CODE_AUTO){
				tmpCodeData = 0;
				blinkCmdFlag = 0x0f;	//
				fndData[3] = 0x00;
				fndData[2] = fndTableChar['N' - 'A'];
				fndData[1] = fndTableChar['O' - 'A'];
				fndData[0] = 0x00;
			} else if ( codeNumber == CODE_SETT ) {
				setEditTmpFndData(tmpCodeData);
			}
		}
	} else if(cmd == BTN_ENTER ){
		machineState = mStateBk;
	}
	return 0;
}

void displayErr(int errNo)
{
	fndData[3] = fndTableChar['E' - 'A'];
	fndData[2] = 0x40;
	fndData[1] = fndTableNum[errNo/10];
	fndData[0] = fndTableNum[(errNo % 10) ];
	blinkCmdFlag = 0x0f;	//
}
