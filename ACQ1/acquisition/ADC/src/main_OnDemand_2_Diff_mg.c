#include <asf.h>
#include <string.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define ADC_TOP_VALUE 2047
#define ADC_REF_VOLTAGE_mV 1000
#define ADC_GAIN 64
#define SAMPLES_PER_MEASUREMENT 2048


static void adc_init(void)
{
	struct adc_config adc_conf; //konfiguracja adc 
	struct adc_channel_config adcch_conf; //konfiguracja kana³u adc  Channels in an ADC is the number of analog values that can be read at the same time.
	
	adc_read_configuration(&MY_ADC, &adc_conf); //czyta aktualn¹ konfiguracje wskazanego adc i zapisuje j¹ do bufora pod wskazanym adresem 
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf); //czyta aktualn¹ konfiguracje wskazanego kana³u adc i zapisuje j¹ do bufora pod wskazanym adresem 
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	ADC_REF_BANDGAP); //pod wskazan¹ konfiguracjê wpisujemy: wynik z znakiem, rozdzielczoœæ 12 bitów,  vref = 1V; rozdzielczosc moze byc jeszcze 8 bitowa lub 12 left-adjusted, LSB = Vref/ 2^res = 0.24mV
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 2, 0); //ustawienia triggera, jest on rêczny, liczba kana³ów 1, kana³ów wyzwalanych wydarzeniem - 0
	adc_set_clock_rate(&adc_conf, 500000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN5, ADCCH_NEG_PIN1, ADC_GAIN); 
	
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
	
	float result,result_mv,result_mg;
	char ucresult[15], *pcDot; //8 cyfr przed przecinkiem, 6 po, null
	unsigned short ucRepCounter;
	adc_enable(&MY_ADC);
		
	
	while(1) {
		result = 0 ;
		for(ucRepCounter=0;ucRepCounter<SAMPLES_PER_MEASUREMENT; ucRepCounter++){
		
			adc_start_conversion(&MY_ADC, MY_ADC_CH);
			adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
			result +=  -adc_get_signed_result(&MY_ADC, MY_ADC_CH);

		}
		result /= SAMPLES_PER_MEASUREMENT;
		result_mv = (result * ADC_REF_VOLTAGE_mV)/((ADC_TOP_VALUE+1));
		result_mv /= ADC_GAIN;
		result_mg = 128571.0*(result-6.787) - 15571.4;
		sprintf(ucresult, "%f",result_mg);
		pcDot = strchr(ucresult, '.');
	
		if(pcDot)
			*pcDot = ',';
		
		printf(" %s  \n\r" ,ucresult);
	}
}