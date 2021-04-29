#include <LPC21xx.H>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"

#define P10_bm (1<<10)

QueueHandle_t QueueServo;

enum ServoState{CALLIB,IDLE, IN_PROGRESS};
	
	struct Servo {
		enum ServoState eState; 
		unsigned int uiCurrentPosition;
		unsigned int uiDesiredPosition;
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
					
			case IDLE :{
				
					struct Servo sServoBuffer;
					if( xQueueReceive(QueueServo, &sServoBuffer,0) == pdPASS){
						sServo.uiDesiredPosition = sServoBuffer.uiDesiredPosition;
						sServo.eState = sServoBuffer.eState;
					}
					else{
					sServo.eState = IDLE;}
				break;}
				
			case IN_PROGRESS :
				if(sServo.uiCurrentPosition != sServo.uiDesiredPosition){
					LedStepLeft();
					sServo.uiCurrentPosition++;
					sServo.uiCurrentPosition%=48;}
					
				else{
					sServo.eState = IDLE;}
				break;		
								
			default:
				break;
			}
	}

void ServoInit(void /*unsigned int uiServoFrequency*/){
	//zastapiono mechanizmem rtos
	//Timer1Interrupts_Init((1000000/uiServoFrequency), &Automat); //spytac jak szybko na plytce
	QueueServo = xQueueCreate( 20, 12 );
	Led_Init();
	DetectorInit();
	ServoCallib();
	QueueServo = xQueueCreate( 20, 12 );
}
	
void ServoCallib(void){
	
	//sServo.eState = CALLIB;
	struct Servo sServoQ = {CALLIB,0 , 0};
  xQueueSend(QueueServo, &sServoQ, 0);
	
}

void ServoGoTo(unsigned int uiPosition){
	
	sServo.uiDesiredPosition = uiPosition;
	sServo.eState = IN_PROGRESS;

}

void ServoCtr(void *pvParameters){

	while(1){
		Automat();
		vTaskDelay(*((TickType_t*)pvParameters));
	}
}
