
#include <asf.h>
#include <delay.h>
#define MY_LED    IOPORT_CREATE_PIN(PORTD, 5)
#define MY_BUTTON IOPORT_CREATE_PIN(PORTE, 5)

int main (void)
{
	
	ioport_init();
	ioport_set_pin_dir(MY_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(MY_BUTTON, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(MY_BUTTON, IOPORT_MODE_PULLUP);
	bool value=1;
	
	while(1){
		
		if(!ioport_get_pin_level(MY_BUTTON)) {
			ioport_set_pin_level(MY_LED, value);
			value = !value;
			delay_ms(100); 
			}
	}
}
