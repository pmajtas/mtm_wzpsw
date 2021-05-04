#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "keyboard.h"
#include "semphr.h"
#include "string.h"

xSemaphoreHandle xSemaphore;


void Rtos_Transmitter_SendString( char ucStringToSend[]){

		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		Transmiter_SendString(ucStringToSend);
		while (Transmiter_GetStatus()!=FREE){};
		xSemaphoreGive( xSemaphore);
}

void LettersTx (void *pvParameters){ //czas na wyslanie bez wcisnietego przycisku : 0x029A ; z wcisnietym przyciskiem:   0x02FF
																											
	char cTxStr[TRANSMITER_SIZE];
	TickType_t uiTime=1,uiStart=0;
	
	while(1){
		CopyString("-ABCDEEFGH-:", cTxStr);
		AppendUIntToString(uiTime , cTxStr);
		AppendString("\n",cTxStr);
		uiStart = xTaskGetTickCount();
		Rtos_Transmitter_SendString(cTxStr);
		uiTime = xTaskGetTickCount() - uiStart;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	while(1){
		
		while(eKeyboardRead() == RELEASED){}
		Rtos_Transmitter_SendString("-Keyboard-\n");
		vTaskDelay(300);
	}
}

int main( void ){
	
	vSemaphoreCreateBinary( xSemaphore );
	
	KeyboardInit();
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1);
	}	

