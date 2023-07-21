//*******************************************
//Lab 7, Section 1
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: setups photoresistor to connect to ADC module A
//*******************************************

#include <avr/io.h>
//definitions
int main(void)
{
	int16_t upperbyte = 0;
	int16_t lowerbyte =0;
	int16_t data = 0;
	//initialize ADC
	adc_init();
    while (1) 
    {
		//begin adca conversion on channel 0
		ADCA.CH0.CTRL |= (ADC_CH_START_bm);
		//check if interrupt flag set
		while(!(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm))
		{
			//do nothing
		}
		//clear interrupt flag
		ADCA.CH0.INTFLAGS = ADC_CH_CHIF_bm;
		//store result
		data = (ADCA.CH0.RES);
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
	ADCA.CH0.CTRL = (ADC_CH_INPUTMODE_DIFFWGAIN_gc);
	//lower ADCA sampling
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
	//set adca muxpos to port A pin 1 and pin 6
	ADCA.CH0.MUXCTRL = (ADC_CH_MUXPOS_PIN1_gc|ADC_CH_MUXNEG_PIN6_gc);
	//ENABLE ADC
	ADCA.CTRLA = (ADC_ENABLE_bm);
}