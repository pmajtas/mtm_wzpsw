#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "keyboard.h"


void LettersTx (void *pvParameters){
	
	while(1){
		Transmiter_SendString("-ABCDEEFGH-\n");
		while (Transmiter_GetStatus()!=FREE){};
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	while(1){
		
		while(eKeyboardRead() == RELEASED){}
		Transmiter_SendString("-Keyboard-\n");
		while (Transmiter_GetStatus()!=FREE){};
		vTaskDelay(300);
	}
}

int main( void ){
	
	KeyboardInit();
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1);
	}	

