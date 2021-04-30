#include <LPC21xx.H>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"

#define P10_bm (1<<10)

QueueHandle_t QueueServo;

enum ServoState{CALLIB, IN_PROGRESS, IDLE, WAIT};
	
	struct Servo {
		enum ServoState eState; 
		unsigned int uiCurrentPosition;
		unsigned int uiDesiredPosition;
		unsigned int uiWaitTicks;
	};
	
	struct Servo sServo;
	

void DetectorInit(void){
	IO0DIR = IO0DIR & ~(P10_bm);
}

enum DetectorState{INACTIVE, ACTIVE} eReadDetector(void){
	if((IO0PIN & P10_bm) == 0){
		return ACTIVE;
	}
	return INACTIVE;
}

void Automat(void){


		switch(sServo.eState){
			
			case CALLIB:
				if(eReadDetector() != ACTIVE){
					sServo.eState = CALLIB;
					LedStepRight();}
				else{
					sServo.uiCurrentPosition=0;
					sServo.uiDesiredPosition=0;
					sServo.eState = IDLE;}
				break;
				
			case IN_PROGRESS :
				if(sServo.uiCurrentPosition != sServo.uiDesiredPosition){
					LedStepLeft();
					sServo.uiCurrentPosition++;
					sServo.uiCurrentPosition%=48;}
					
				else
					sServo.eState = IDLE;
					
				break;	

			case IDLE:{
				struct Servo sServoBuffer;
				xQueueReceive(QueueServo, &sServoBuffer,portMAX_DELAY); //czekanie az bedzie sie dalo odczytac z kolejki
				sServo.uiDesiredPosition = sServoBuffer.uiDesiredPosition;
				sServo.eState = sServoBuffer.eState;
				break;
				}
								
			default:
				break;
			}
	}

void ServoInit(void ){

	QueueServo = xQueueCreate( 16, 16 ); 
	Led_Init();
	DetectorInit();
	ServoCallib();
}
	
void ServoCallib(void){
	
	struct Servo sServoQ = {CALLIB,0 , 0, 0};
  xQueueSend(QueueServo, &sServoQ, 0);
	
}

void ServoGoTo(unsigned int uiPosition){
	
	struct Servo sServoQ = {IN_PROGRESS, 0 , 0, 0};
	sServoQ.uiDesiredPosition = uiPosition;
  xQueueSend(QueueServo, &sServoQ, 0);

}



void ServoCtr(void *pvParameters){

	while(1){
		Automat();
		vTaskDelay(*((TickType_t*)pvParameters));
	}
}
