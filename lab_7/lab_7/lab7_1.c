//*******************************************
//Lab 7, Section 1
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: setups photoresistor to connect to ADC module A
//*******************************************

#include <avr/io.h>
#include "usart.h"
//definitions
#define ADC_CONVMODE_NORMAL_BM (0X01<<4)
int main(void)
{
	int16_t upperbyte = 0;
	int16_t lowerbyte =0;
	uint8_t upperbyte_usart = 0;
	uint8_t lowerbyte_usart =0;
	int16_t data = 0;
	//initialize ADC
	adc_init();
	usartd0_init();
    while (1) 
    {
		//begin adca conversion on channel 0
		ADCA.CH0.CTRL = (ADCA.CH0.CTRL|ADC_CH_START_bm);
		//check if interrupt flag set
		while(!(ADCA.CH0.INTFLAGS & ADCA_CH0_INTFLAGS))
		{
			//do nothing
		}
		//store result
		upperbyte = (ADCA.CH0.RESH<<7);
		lowerbyte = (ADCA.CH0.RESL<<0);
		data = (upperbyte|lowerbyte);
		//TEST CODE
		upperbyte_usart = (ADCA.CH0.RESH<<0);
		lowerbyte_usart = (ADCA.CH0.RESL<<0);
		usartd0_out_char(lowerbyte_usart);
		usartd0_out_char(upperbyte_usart);
    }
}

void adc_init(void)
{
	
	//set port a pin 1 and 6 as inputs
	PORTA.DIRCLR = (PIN1_bm|PIN6_bm);
	//set adca as 12 bit signed right adjusted
	ADCA.CTRLB = (ADC_CONVMODE_NORMAL_BM|ADC_RESOLUTION_12BIT_gc);
	//set adca reference voltage to aref
	ADCA.REFCTRL = (0|ADC_REFSEL_AREFA_gc);
	//set adca channel 0 to differential with gain x1
	ADCA.CH0.CTRL = (ADC_CH_INPUTMODE_DIFFWGAIN_gc|ADC_CH_GAIN1_bm);
	//set adca muxpos to port A pin 1 and pin 6
	ADCA.CH0.MUXCTRL = (ADC_CH_MUXPOS1_bm|ADC_CH_MUXNEG_PIN6_gc);
	//load calibration register with factory values
	ADCA.CALL = ADCA_CALL;
	ADCA.CALH = ADCA_CALH;
	//ENABLE ADC
	ADCA.CTRLA = (ADCA.CTRLA|ADC_ENABLE_bm);
}