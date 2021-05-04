#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"

xSemaphoreHandle xSemaphore;

void PulseTrigger( void *pvParameters ){
	
	while(1){
			xSemaphoreGive(xSemaphore);
			vTaskDelay(1000);
	}
}


void Pulse_LED0( void *pvParameters ){
	
	while(1){
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		Led_Set(0);
		vTaskDelay(100);
		Led_Clr(0);
	}
}



int main( void ){
	
	vSemaphoreCreateBinary( xSemaphore );
	Led_Init();
	xTaskCreate(PulseTrigger, NULL , 100 ,NULL, 2 , NULL );
	xTaskCreate(Pulse_LED0, NULL , 100 , NULL, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

