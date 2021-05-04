
#include "led.h"

void Delay(unsigned int uiMiliSec) {

unsigned int uiLoopCtr, uiDelayLoopCount;
uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr=0;uiLoopCtr<uiDelayLoopCount;uiLoopCtr++) {}

}

int main( void ){
	unsigned char ucPeriodCounter1=0, ucPeriodCounter2=0;
	Led_Init();
	
	while(1){
		if(!ucPeriodCounter1)
			Led_Toggle(0);
		if(!ucPeriodCounter2)
			Led_Toggle(1);
		ucPeriodCounter1 = (ucPeriodCounter1+1)%166;
		ucPeriodCounter2 = (ucPeriodCounter2+1)%125;
		Delay(1);
	}
}
