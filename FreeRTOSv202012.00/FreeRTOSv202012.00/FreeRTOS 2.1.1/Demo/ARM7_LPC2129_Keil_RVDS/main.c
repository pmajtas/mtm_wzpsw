#include "led.h"

void Delay(unsigned int uiMiliSec) {

unsigned int uiLoopCtr, uiDelayLoopCount;
uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr=0;uiLoopCtr<uiDelayLoopCount;uiLoopCtr++) {}

}

int main( void ){
	unsigned char ucPeriodCounter;
	Led_Init();
	
	while(1){
		Led_Toggle(0);
		for( ucPeriodCounter = 0; ucPeriodCounter < 4; ucPeriodCounter++){
		Led_Toggle(1);
		Delay(125);
		}
	}
}
