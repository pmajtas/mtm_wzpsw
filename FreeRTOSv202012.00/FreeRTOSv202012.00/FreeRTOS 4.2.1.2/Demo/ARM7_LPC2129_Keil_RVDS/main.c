#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "keyboard.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "led.h"

QueueHandle_t QueueUart;


void Rtos_Transmiter_SendString( void *pvParameters){

	char cStringToSend[TRANSMITER_SIZE];
	while(1){
		xQueueReceive(QueueUart, cStringToSend,0);
		Transmiter_SendString(cStringToSend);
		while (Transmiter_GetStatus()!=FREE){};
		}
}

void LettersTx (void *pvParameters){
	
	char cTxStr[TRANSMITER_SIZE];
	TickType_t uiTime=1,uiStart=0;
	
	while(1){
		CopyString("-ABCDEEFGH-:", cTxStr);
		AppendUIntToString(uiTime , cTxStr);
		AppendString("\n",cTxStr);
		uiStart = xTaskGetTickCount();
		if (errQUEUE_FULL == xQueueSend(QueueUart, cTxStr, 0))
		{
				Led_Toggle(0);}
				
		uiTime = xTaskGetTickCount() - uiStart;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	while(1){
		
		while(eKeyboardRead() == RELEASED){}
		xQueueSend(QueueUart, "-Keyboard-\n",0);
		vTaskDelay(300);
	}
}

int main( void ){
	
	
	KeyboardInit();
	Led_Init();
	UART_InitWithInt(300);
	QueueUart = xQueueCreate(5, 20);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(Rtos_Transmiter_SendString, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1);
	}	

