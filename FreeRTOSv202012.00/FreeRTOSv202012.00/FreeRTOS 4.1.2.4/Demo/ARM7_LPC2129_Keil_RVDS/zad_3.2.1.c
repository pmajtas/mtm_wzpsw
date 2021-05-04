#include "FreeRTOS.h"
#include "task.h"
#include "led.h"


void LedBlink( void *pvParameters ){
	
	unsigned char ucFreq = *((unsigned char*)pvParameters);
	
	while(1){
		Led_Toggle(0);
		vTaskDelay(1000/ucFreq/2);
	}
}

void LedCtrl( void *pvParameters ){

	TaskHandle_t thLedBlink = *((TaskHandle_t*)pvParameters);
	
	while(1){
			vTaskSuspend(thLedBlink);
			vTaskDelay(1000);
			vTaskResume(thLedBlink);
			vTaskDelay(1000);
	}
}

int main( void ){
	
	unsigned char ucFreq = 4;
	TaskHandle_t thLedBlink;
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &ucFreq, 2 , &thLedBlink );
	xTaskCreate(LedCtrl, NULL , 100 , &thLedBlink, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

