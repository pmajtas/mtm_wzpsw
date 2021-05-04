#include <LPC21xx.H>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"

#define P10_bm (1<<10)

QueueHandle_t QueueServo;

enum ServoState{CALLIB, IN_PROGRESS, IDLE};
enum Command {CAL, GO,WAIT,SPEED};
	
	struct Servo {
		enum ServoState eState; 
		unsigned int uiCurrentPosition;
		unsigned int uiDesiredPosition;
		unsigned int uiWaitTicks;
		unsigned int uiStepTicks;
	};
	
	struct ServoCommand{
		enum Command eCommand;
		unsigned int uiValue;
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
				struct ServoCommand sServoBuffer;
				xQueueReceive(QueueServo, &sServoBuffer,portMAX_DELAY); //czekanie az bedzie sie dalo odczytac z kolejki

					switch(sServoBuffer.eCommand){
						
						case GO:
							sServo.eState = IN_PROGRESS;
							sServo.uiDesiredPosition = sServoBuffer.uiValue;
							break;
						
						case WAIT:
							sServo.eState = IDLE;
							vTaskDelay(sServoBuffer.uiValue);
							break;
						
						case SPEED:
							sServo.eState = IDLE;
							sServo.uiStepTicks = sServoBuffer.uiValue;
							break;	
						
						case CAL:
							sServo.eState = CALLIB;
							break;
						
						default:
							sServo.eState = IDLE;
							break;
							
					}
				break;
				}
								
			default:
				sServo.eState = IDLE;
				break;
			}
		vTaskDelay(sServo.uiStepTicks);
	}

void ServoInit(void ){
	
	QueueServo = xQueueCreate( 16, 8 ); 
	Led_Init();
	DetectorInit();
	ServoCallib();
	xTaskCreate(ServoCtr, NULL, 127, NULL, 1, NULL );
}
	
void ServoCallib(void){
	
	struct ServoCommand sServoBuffer = {CAL,0	};
  xQueueSend(QueueServo, &sServoBuffer, 0);
	
}

void ServoGoTo(unsigned int uiPosition){
	
	struct ServoCommand sServoBuffer = {GO, 0};
	sServoBuffer.uiValue = uiPosition;
  xQueueSend(QueueServo, &sServoBuffer, 0);

}

void ServoWait(unsigned int uiTicksToWait){

	struct ServoCommand sServoBuffer = {WAIT,0};
	sServoBuffer.uiValue= uiTicksToWait;
  xQueueSend(QueueServo, &sServoBuffer, 0);

}

void ServoSpeed(unsigned int uiTicksStep){

	struct ServoCommand sServoBuffer = {SPEED, 0 };
	sServoBuffer.uiValue = uiTicksStep;
  xQueueSend(QueueServo, &sServoBuffer, 0);
		
}



void ServoCtr(void *pvParameters){

	while(1){
		Automat();
		
	}
}
