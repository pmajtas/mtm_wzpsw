#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void Delay(unsigned int uiMiliSec) {

unsigned int uiLoopCtr, uiDelayLoopCount;
uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr=0;uiLoopCtr<uiDelayLoopCount;uiLoopCtr++) {}

}

void LedBlink( void *pvParameters ){
	
	while(1){
		Led_Toggle(0);
		vTaskDelay((1000/ (*((unsigned char*)pvParameters)))/2);
	}
}

void LedCtrl( void *pvParameters ){

	while(1){
	*((unsigned char *)pvParameters) = 2;
	vTaskDelay(1000);
	*((unsigned char *)pvParameters) = 10;
	vTaskDelay(1000);
	}
}

int main( void ){
	
	unsigned char ucBlinkingFreq = 1;
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &ucBlinkingFreq, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

