#include <asf.h>
#include <string.h>


#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

//ADC config
#define ADC_TOP_VALUE 2047
#define ADC_REF_VOLTAGE_mV 1000
#define ADC_GAIN 64
#define SAMPLES_PER_MEASUREMENT 2048

//Float to string parameters
#define  VOLTAGE_STRING_LENGTH 16

//STRAIN parameters
#define STRAIN_slope_factor 123.8080
#define STRAIN_offset -845.3450
#define STRAIN_mV_to_mg (ADC_TOP_VALUE+1)*(ADC_GAIN)/ADC_REF_VOLTAGE_mV *STRAIN_slope_factor


static void adc_init(void)
{
	struct adc_config adc_conf; //konfiguracja adc
	struct adc_channel_config adcch_conf; //konfiguracja kana³u adc  Channels in an ADC is the number of analog values that can be read at the same time.
	
	adc_read_configuration(&MY_ADC, &adc_conf); //czyta aktualn¹ konfiguracje wskazanego adc i zapisuje j¹ do bufora pod wskazanym adresem
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf); //czyta aktualn¹ konfiguracje wskazanego kana³u adc i zapisuje j¹ do bufora pod wskazanym adresem
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	ADC_REF_BANDGAP); //pod wskazan¹ konfiguracjê wpisujemy: wynik z znakiem, rozdzielczoœæ 12 bitów,  vref = 1V; rozdzielczosc moze byc jeszcze 8 bitowa lub 12 left-adjusted, LSB = Vref/ 2^res = 0.24mV
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0); //ustawienia triggera, jest on rêczny, liczba kana³ów 1, nr kana³u 0
	adc_set_clock_rate(&adc_conf, 500000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, ADC_GAIN);
	
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
	
	float fResult=0,fResult_mv,fResult_mg;
	char ucResult[VOLTAGE_STRING_LENGTH], *pcDot; 
	unsigned short usRepCounter;
	char cDemand;
	adc_enable(&MY_ADC);
	
	
	while(1) {
		
		
		scanf("%c", &cDemand); //czekanie na jakis znak
		fResult =0;
		for(usRepCounter=0;usRepCounter<SAMPLES_PER_MEASUREMENT; usRepCounter++){
			
			adc_start_conversion(&MY_ADC, MY_ADC_CH);
			adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
			fResult +=  adc_get_signed_result(&MY_ADC, MY_ADC_CH);

		}
		fResult /= SAMPLES_PER_MEASUREMENT;
		fResult_mv = (fResult * ADC_REF_VOLTAGE_mV)/((ADC_TOP_VALUE+1));
		fResult_mv /= ADC_GAIN;
		fResult_mg = fResult_mv*STRAIN_mV_to_mg + STRAIN_offset;
		
		sprintf(ucResult, "%f",fResult_mv);
		pcDot = strchr(ucResult, '.');
		if(pcDot)
		*pcDot = ',';
		
		printf(" %s  \n\r" ,ucResult);
	}
}