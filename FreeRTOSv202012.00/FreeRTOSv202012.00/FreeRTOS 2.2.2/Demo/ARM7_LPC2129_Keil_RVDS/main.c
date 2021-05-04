#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void Delay(unsigned int uiMiliSec) {

unsigned int uiLoopCtr, uiDelayLoopCount;
uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr=0;uiLoopCtr<uiDelayLoopCount;uiLoopCtr++) {}

}

void Led0Blink( void *pvParameters ){

	while(1){
	Led_Toggle(0);
	vTaskDelay(1000);
	}
}

void Led1Blink( void *pvParameters ){

	while(1){
	Led_Toggle(1);
	vTaskDelay(50); //po zamianie na vTaskDelay dzialanie przyspieszylo dwukrotnie
	}
}
int main(void){
	
	Led_Init();
	xTaskCreate(Led0Blink, NULL , 100 , NULL, 2 , NULL ); //100 glebokosc sterty, 2 priorytet
	xTaskCreate(Led1Blink, NULL , 100 , NULL, 2 , NULL ); //100 glebokosc sterty, 2 priorytet
	vTaskStartScheduler();
	
	while(1);

}

