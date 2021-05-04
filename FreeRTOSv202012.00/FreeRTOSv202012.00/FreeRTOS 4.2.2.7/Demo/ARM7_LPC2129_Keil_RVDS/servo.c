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
		unsigned int uiStepTicks;
	};
	
	struct Servo sServo = 	{IDLE,0,0,0,100} ;

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
				sServo.uiWaitTicks = sServoBuffer.uiWaitTicks;
				sServo.eState = sServoBuffer.eState;
				if( sServoBuffer.uiStepTicks !=0 )
					sServo.uiStepTicks = sServoBuffer.uiStepTicks;
				break;
				}
				
			case WAIT:
				vTaskDelay(sServo.uiWaitTicks);
				sServo.eState = IDLE;
				break;
								
			default:
				sServo.eState = IDLE;
				break;
			}
		vTaskDelay(sServo.uiStepTicks);
	}

void ServoInit(void ){
	
	QueueServo = xQueueCreate( 16, 20 ); 
	Led_Init();
	DetectorInit();
	ServoCallib();
	xTaskCreate(ServoCtr, NULL, 127, NULL, 1, NULL );
}
	
void ServoCallib(void){
	
	struct Servo sServoBuffer = {CALLIB,0 , 0, 0, 100	};
  xQueueSend(QueueServo, &sServoBuffer, 0);
	
}

void ServoGoTo(unsigned int uiPosition){
	
	struct Servo sServoBuffer = {IN_PROGRESS, 0 , 0, 0, 0};
	sServoBuffer.uiDesiredPosition = uiPosition;
  xQueueSend(QueueServo, &sServoBuffer, 0);

}

void ServoWait(unsigned int uiTicksToWait){

	struct Servo sServoBuffer = {WAIT, 0 , 0, 0, 0};
	sServoBuffer.uiWaitTicks = uiTicksToWait;
  xQueueSend(QueueServo, &sServoBuffer, 0);

}

void ServoSpeed(unsigned int uiTicksStep){

	struct Servo sServoBuffer = {IDLE, 0 , 0, 0, 0};
	sServoBuffer.uiStepTicks = uiTicksStep;
  xQueueSend(QueueServo, &sServoBuffer, 0);
		
}



void ServoCtr(void *pvParameters){

	while(1){
		Automat();
		
	}
}
