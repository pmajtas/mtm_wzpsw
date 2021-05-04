#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

typedef struct ToggleConfig {
	unsigned char ucFreq;
	unsigned char ucLedIndex;
} ToggleConfig;


void LedBlink( void *pvParameters ){
	
	while(1){
		Led_Toggle(((struct ToggleConfig*)pvParameters)->ucLedIndex);
		vTaskDelay((1000/(((struct ToggleConfig*)pvParameters)->ucFreq))/2);
	}
}

void LedCtrl( void *pvParameters ){

	unsigned char ucLedIndex;
	
	while(1){
		
	for(ucLedIndex =0; ucLedIndex<2; ucLedIndex++){
		(((struct ToggleConfig*)pvParameters)->ucLedIndex) = ucLedIndex;
		(((struct ToggleConfig*)pvParameters)->ucFreq) = 2;
		vTaskDelay(1000);
		(((struct ToggleConfig*)pvParameters)->ucFreq) = 10;
		vTaskDelay(1000);
		
		}
	}
}

int main( void ){
	
	ToggleConfig sToggleConfig = {2,0};
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &sToggleConfig, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &sToggleConfig, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

