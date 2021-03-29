#include <asf.h>
#include <string.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define ADC_TOP_VALUE 255
#define ADC_REF_VOLTAGE_mV 3300
#define ADC_GAIN 1


static void adc_init(void)
{
	struct adc_config adc_conf; //konfiguracja adc
	struct adc_channel_config adcch_conf; //konfiguracja kana³u adc  Channels in an ADC is the number of analog values that can be read at the same time.
	
	adc_read_configuration(&MY_ADC, &adc_conf); //czyta aktualn¹ konfiguracje wskazanego adc i zapisuje j¹ do bufora pod wskazanym adresem
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf); //czyta aktualn¹ konfiguracje wskazanego kana³u adc i zapisuje j¹ do bufora pod wskazanym adresem
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_8,
	ADC_REF_AREFA); //pod wskazan¹ konfiguracjê wpisujemy: wynik bez znaku, rozdzielczoœæ 8 bitów,  vref zewnêtrzny z portu A; rozdzielczosc moze byc jeszcze 8 bitowa lub 12 left-adjusted, LSB = Vref/ 2^res = 0.24mV
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0); //ustawienia triggera, jest on rêczny, liczba kana³ów 1, kana³ów wyzwalanych wydarzeniem - 0
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, ADC_GAIN);
	
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

int main (void) {
	
	sysclk_init();
	adc_init();
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	unsigned char result;
	adc_enable(&MY_ADC);
	
	
	while(1) {
			adc_start_conversion(&MY_ADC, MY_ADC_CH);
			adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
			result =  adc_get_result(&MY_ADC, MY_ADC_CH);
			printf(" %d  \n\r" ,result);
	}
}