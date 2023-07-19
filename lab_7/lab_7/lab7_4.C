//*******************************************
//Lab 7, Section 4
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: samples the photoresistor 137 times per second and outputs it to the computer
//*******************************************
//includes
#include <avr/interrupt.h>
#include <avr/io.h>
//variables
volatile int16_t result;
volatile int8_t bsel = 5;
volatile int8_t bscale = -6;
volatile uint8_t adca_ready;
int main(void)
{
	int8_t upperbyte = 0;
	int8_t lowerbyte =0;
	//initialize ADC
	adc_init();
	//initialize tcc0
	tcc0_init();
	//initialize interrupts
	inter_init();
	//initialize usartd0
	usartd0_init();
	//start timer counter
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	while (1)
	{
		if(adca_ready)
		{
			upperbyte = (ADCA.CH0.RESH<<0);
			lowerbyte = (ADCA.CH0.RESL<<0);
			//output adca result to computer
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = lowerbyte;
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = upperbyte;
			//reset adca
			adca_ready = 0;
		}
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

void inter_init(void)
{
	//enable medium level interrupts in pmic
	PMIC.CTRL = PMIC_MEDLVLEN_bm;
	//enable global interrupts
	sei();
}
void tcc0_init(void)
{
	uint16_t period =  115;
	uint8_t offset = 0;
	//load tcc0 period register
	TCC0.PER = period + offset;
	TCC0.CNT = 0;
	//set tcc0 as event channel 0 source
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
}

void usartd0_init(void)
{
  //initialize transmitter and reciever pins
	PORTD.OUTSET = PIN3_bm;
	PORTD.DIRSET = PIN3_bm;
	PORTD.DIRCLR = PIN2_bm;

  //set baud rate
	USARTD0.BAUDCTRLA = (uint8_t)bsel;
	USARTD0.BAUDCTRLB = (uint8_t)((bscale << 4)|(00 >> 4));

  //set to 8 bit odd parity with 1 stop bit
	USARTD0.CTRLC =	(USART_CMODE_ASYNCHRONOUS_gc |USART_PMODE_ODD_gc| USART_CHSIZE_8BIT_gc)&(~USART_SBMODE_bm);

  //ENABLE TRANSMITTER AND RECIEVER
	USARTD0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

ISR(ADCA_CH0_vect)
{
	result = (ADCA.CH0.RESH<<8 | ADCA.CH0.RESL<<0);
	//set ADCA_READY flag
	adca_ready = 1;
	
}