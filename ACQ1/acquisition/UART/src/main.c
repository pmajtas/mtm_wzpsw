#include <asf.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)

int main (void) {
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);

	
	//unsigned char ch;
	int i=0;
	while(1) {
		
		/*scanf("%c",&ch); //formatted output - 
		if(ch == 0x62)
			printf("\n\r binary \n\r");
		else if(ch == 0x76)
			printf("\n\r voltage \n\r");
		else if(ch == 0x74)
			printf("\n\r temperature \n\r");
		else
			printf("\n\r ??? \n\r");
		printf("%d\n", i);*/
		i++;


	}
}