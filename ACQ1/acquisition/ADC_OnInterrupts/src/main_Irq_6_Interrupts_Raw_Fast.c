#include <asf.h>
#include <string.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

//ADC config
#define ADC_GAIN 1

//TCC0 
#define MY_TIMER TCC0

static void tc_init(void)
{
	tc_enable(&MY_TIMER); //wlaczenie timera
	tc_set_wgm(&MY_TIMER, TC_WG_NORMAL); //konfiguruje TC na tryb normalny
	tc_write_period(&MY_TIMER, 10); //chcemy miec 10Hz czyli okres 50kHz
	tc_set_resolution(&MY_TIMER, 500000); //rozdzielczosc Timera
}

static void evsys_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc; //

}

static void ADC_CALLBACK(ADC_t *adc, uint8_t ch_mask, adc_result_t result){

	putchar((unsigned char)result);
}

static void adc_init(void)
{
	struct adc_config adc_conf; //konfiguracja adc
	struct adc_channel_config adcch_conf; //konfiguracja kana³u adc  Channels in an ADC is the number of analog values that can be read at the same time.
	
	adc_read_configuration(&MY_ADC, &adc_conf); //czyta aktualn¹ konfiguracje wskazanego adc i zapisuje j¹ do bufora pod wskazanym adresem
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf); //czyta aktualn¹ konfiguracje wskazanego kana³u adc i zapisuje j¹ do bufora pod wskazanym adresem
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_8, ADC_REF_AREFA); //pod wskazan¹ konfiguracjê wpisujemy: wynik bez znaku, rozdzielczoœæ 8 bitów,  vref zewnêtrzny z portu A; rozdzielczosc moze byc jeszcze 8 bitowa lub 12 left-adjusted, LSB = Vref/ 2^res = 0.24mV
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_EVENT_SINGLE, 1, 0); 
	adc_set_clock_rate(&adc_conf, 500000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, ADC_GAIN);
	adcch_enable_interrupt(&adcch_conf);
	adc_set_callback(&MY_ADC, &ADC_CALLBACK);


	
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

int main (void) {
	
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 921600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	cpu_irq_enable();
	pmic_init();
	sysclk_init();
	evsys_init();
	tc_init();
	adc_init();	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	adc_enable(&MY_ADC);
		
	while(1) {	


	}
}