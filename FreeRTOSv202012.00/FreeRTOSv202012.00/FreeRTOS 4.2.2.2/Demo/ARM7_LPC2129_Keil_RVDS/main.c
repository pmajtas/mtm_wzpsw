#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "keyboard.h"
#include "led.h"
#include "servo.h"

void Keyboard (void *pvParameters){
	enum eKeyboardButtons eButton;
	
	while(1){
		eButton = eKeyboardRead();
		while(eKeyboardRead() != RELEASED){}
		switch(eButton){
			case BUTTON_0: ServoCallib();
				break;
			
			case BUTTON_1: ServoGoTo(12);
				break;
			
			case BUTTON_2: ServoGoTo(24);
				break;
			
			case BUTTON_3: ServoGoTo(36);
				break;
			
			default:
				break;
		}
		while(eKeyboardRead() != RELEASED){}
	vTaskDelay(100);
	}
}


int main( void ){
	
	TickType_t TTperiod=50;
	KeyboardInit();
	ServoInit(10);

	xTaskCreate(Keyboard, NULL, 127, NULL, 1, NULL );
	xTaskCreate(ServoCtr, NULL, 127, &TTperiod, 1, NULL );
	vTaskStartScheduler();
	
	while(1);
	}	

