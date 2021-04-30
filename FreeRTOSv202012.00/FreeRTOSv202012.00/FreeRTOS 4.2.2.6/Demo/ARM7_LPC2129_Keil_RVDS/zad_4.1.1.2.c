#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"


void PulseTrigger( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle*)pvParameters);
	
	while(1){
			xSemaphoreGive(xSemaphore);
			vTaskDelay(1000);
	}
}

void PulseTrigger3( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle*)pvParameters);
	
	vTaskDelay(333);
	while(1){
			xSemaphoreGive(xSemaphore);
			vTaskDelay(333);
	}
}

void Pulse_LED0( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle*)pvParameters);
	
	while(1){
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		Led_Set(0);
		vTaskDelay(100);
		Led_Clr(0);
	}
}



int main( void ){
	
	xSemaphoreHandle xSemaphore;
	vSemaphoreCreateBinary( xSemaphore );
	Led_Init();
	xTaskCreate(PulseTrigger, NULL , 100 , &xSemaphore, 2 , NULL );
	xTaskCreate(PulseTrigger3, NULL , 100 , &xSemaphore, 2 , NULL );
	xTaskCreate(Pulse_LED0, NULL , 100 ,  &xSemaphore, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

