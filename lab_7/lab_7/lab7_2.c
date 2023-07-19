//*******************************************
//Lab 7, Section 2
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: samples the photoresistory six times per second
//*******************************************
//includes
#include <avr/interrupt.h>
#include <avr/io.h>
//variables
volatile int16_t result;
int main(void)
{
	//initialize ADC
	adc_init();
	//initialize interrupts
	inter_init();
	//initialize tcc0
	tcc0_init();
	//init red led
	PORTD.DIRSET = PIN4_bm;
	//start timer counter
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
	while (1)
	{
		//do nothing
	}
}

void adc_init(void)
{
	//set port a pin 1 and 6 as inputs
	PORTA.DIRCLR = (PIN1_bm|PIN6_bm);
	//set adca as 12 bit signed right adjusted
	ADCA.CTRLB = (ADC_CONMODE_bm|ADC_RESOLUTION_12BIT_gc);
	//set adca reference voltage to +2.5V
	ADCA.REFCTRL = (0|ADC_REFSEL_AREFB_gc);
	//set adca channel 0 to differential with gain x1
	ADCA.CH0.CTRL = (ADC_CH_INPUTMODE_DIFFWGAIN_gc|ADC_CH_GAIN1_bm);
	//lower ADCA sampling
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
	//set adca muxpos to port A pin 1 and pin 6
	ADCA.CH0.MUXCTRL = (ADC_CH_MUXPOS_PIN1_gc|ADC_CH_MUXNEG_PIN6_gc);
	//enable interrupts on ADCA ch0
	ADCA.CH0.INTCTRL = (ADC_CH_INTMODE_COMPLETE_gc|ADC_CH_INTLVL_MED_gc);
	//enable conversion trigger on channel event 0
	ADCA.EVCTRL= (ADC_EVSEL_0123_gc|ADC_EVACT_CH0_gc);
	//load calibration register with factory values
	ADCA.CALL = ADCA_CALL;
	ADCA.CALH = ADCA_CALH;
	//ENABLE ADC
	ADCA.CTRLA = (ADC_ENABLE_bm);
}
void tcc0_init(void)
{
	uint8_t period = 159;
	uint8_t offset = 6;
	//load tcc0 period register
	TCC0.PER = period + offset;
	TCC0.CNT = 0;
	//set tcc0 as event channel 0 source
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
}
void inter_init(void)
{
	//enable medium level interrupts in pmic
	PMIC.CTRL = PMIC_MEDLVLEN_bm;
	//enable global interrupts
	sei();
}

ISR(ADCA_CH0_vect)
{
	result = (ADCA.CH0.RESH<<8 | ADCA.CH0.RESL<<0);
	//toggle red pwm led on port D
	PORTD.OUTTGL = PIN4_bm;
	
}