//*******************************************
//Lab 7, Section 3
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: samples the photoresistor every second and outputs it to the computer
//*******************************************
//includes
#include <avr/interrupt.h>
#include <avr/io.h>
//definitions
#define byte1_bm (15 << 4)
#define byte0_bm (15 << 0)
#define resolution (.00122)
#define intercept (.00061)
//variables
volatile int16_t result;
volatile int8_t bsel = 5;
volatile int8_t bscale = -6;
volatile uint8_t adca_ready;
int main(void)
{
 	uint8_t int1 = 0;
	uint8_t int2 = 0;
	uint8_t int3 = 0;
	float result2 =0;
	float result3 =0;
	//initialize ADC
	adc_init();
	//initialize tcc0
	tcc0_init();
	//initialize interrupts
	inter_init();
	//initialize usartd0
	usartd0_init();
	//start timer counter
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
	while (1)
	{
		if(adca_ready)
		{
			//output ADCA result to computer
			
			////////////////////////////////////
			/*OUTPUT VOLTAGE IN DECIMAL VALUE*/
			////////////////////////////////////
			//output positive or negative sign
			if(result < 0)
			{
				result = result*-1;
				USARTD0.DATA = '-';
				while(!(USARTD0.STATUS & USART_DREIF_bm))
				{
					//do nothing
				}
			}
			else if(result > 0)
			{
				USARTD0.DATA = '+';
				while(!(USARTD0.STATUS & USART_DREIF_bm))
				{
					//do nothing
				}
			}
			//convert to voltage value
			float result_flt = (result*resolution)+(intercept);
			
			//get first digit and transmit
			int1 = (uint8_t)(result_flt);
			USARTD0.DATA = (int1+48);
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			//transmit decimal point
			USARTD0.DATA = '.';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			
			//get second decimal digit and transmit
			result2 =(10*(result_flt-int1));
			int2 = (uint8_t)(result2);
			USARTD0.DATA = (int2+48);
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			
			//get third decimal digit and transmit
			result3 = (10*(result2-int2));
			int3 =(uint8_t)(result3);
			USARTD0.DATA = (int3+48);
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			
			//output voltage symbol
			USARTD0.DATA = 'V';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			////////////////////////////////////
			/*END OUTPUT VOLTAGE IN DECIMAL VALUE*/
			////////////////////////////////////
			
			////////////////////////////////////////
			/*SPACE*/
			////////////////////////////////////
			USARTD0.DATA = ' ';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			////////////////////////////////////////
			/*END SPACE*/
			////////////////////////////////////
			
			////////////////////////////////////////
			/*OUTPUT VOLTAGE IN HEXADECIMAL VALUE*/
			////////////////////////////////////
			USARTD0.DATA = '(';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = '0';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = 'x';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			//CONVERT TO HEX
			//get byte 0
			int1 = (result & byte0_bm);
			if(int1 > 9)
			{
				int1 = int1+55;
			}
			else
			{
				int1 = int1+48;
			}
			//get byte 1
			int2 = (result & byte1_bm);
			int2 = (int2>>4);
			if(int2 > 9)
			{
				int2 = int2+55;
			}
			else
			{
				int2 = int2+48;
			}
			//get byte 3
			int3 = (result >> 8);
			if(int3 > 9)
			{
				int3 = int3+55;
			}
			else
			{
				int3 = int3+48;
			}
			USARTD0.DATA = int3;
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = int2;
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = int1;
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			USARTD0.DATA = ')';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			////////////////////////////////////////
			/*END OUTPUT VOLTAGE IN HEXADECIMAL VALUE*/
			////////////////////////////////////
			
			////////////////////////////////////////
			/*CARRIAGE RETURN*/
			////////////////////////////////////
			USARTD0.DATA = '\r';
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			////////////////////////////////////////
			/*END CARRIAGE RETURN*/
			////////////////////////////////////
			
			////////////////////////////////////////
			/*LINEFEED*/
			////////////////////////////////////
			USARTD0.DATA = 10;
			while(!(USARTD0.STATUS & USART_DREIF_bm))
			{
				//do nothing
			}
			////////////////////////////////////////
			/*END LINEFEED*/
			////////////////////////////////////
			
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
	ADCA.CH0.CTRL = (ADC_CH_INPUTMODE_DIFFWGAIN_gc);
	//lower ADCA sampling
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
	//set adca muxpos to port A pin 1 and pin 6
	ADCA.CH0.MUXCTRL = (ADC_CH_MUXPOS_PIN1_gc|ADC_CH_MUXNEG_PIN6_gc);
	//enable interrupts on ADCA ch0
	ADCA.CH0.INTCTRL = (ADC_CH_INTMODE_COMPLETE_gc|ADC_CH_INTLVL_MED_gc);
	//enable conversion trigger on channel event 0
	ADCA.EVCTRL= (ADC_EVSEL_0123_gc|ADC_EVACT_CH0_gc);
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
	uint16_t period = 977;
	uint8_t offset = 15;
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
	USARTD0.BAUDCTRLB = (uint8_t)((bscale << 4)|(bsel >> 8));

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