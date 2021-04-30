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
				
				case BUTTON_3: {
					unsigned char ucCounter;
					for(ucCounter=1; ucCounter <5; ucCounter++){
						ServoSpeed(120 - ucCounter*25);
						ServoGoTo((ucCounter*12)%48);}
					
					break;
					}
				default:
					break;
			}
			while(eKeyboardRead() != RELEASED){}
		
		
	vTaskDelay(100);
	}
}


int main( void ){
	
	KeyboardInit();
	ServoInit();

	xTaskCreate(Keyboard, NULL, 127, NULL, 1, NULL );
	xTaskCreate(ServoCtr, NULL, 127, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1);
	}	

