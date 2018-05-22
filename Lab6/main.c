/**
 *@file: main.c
 *@brief: Sends value from accelerometer to RBG light 
 *@date: 4/5/2018 1:33:40 PM
 *@author: Robert C. Dansro
 **/ 

#define FOSC 16000000 /*Clock Speed */
#define F_CPU 16000000 /*Clock Speed */
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

extern void write_rgb(uint8_t r, uint8_t g, uint8_t b);
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void ADC_Init(void);
uint8_t ADC_read(uint8_t ch);
void printUART(const char *str);
void callibration(int *rmax, int *rmin,
int *gmax, int *gmin, int *bmax, int *bmin);

int main(void)
{
	int rmax = 0, gmax = 0, bmax = 0, rmin = 255, 
	gmin = 255, bmin = 255;
	int rrange, grange, brange;
	uint8_t r = 0, g = 0, b = 0;

	write_rgb(255, 0, 0); /*Set light to red while calibrating */
	ADC_Init(); /*Initialize the ADC */
	callibration(&rmax, &rmin, &gmax, &gmin, &bmax, &bmin);
	write_rgb(0, 255, 0); /*Set light to green after calibrating */
	_delay_ms(500);
	
	/*Find range/normalization values */
	rrange = 255/(rmax-rmin); 
	grange = 255/(gmax-gmin);
	brange = 255/(bmax-bmin);
	
	while (1) {
		/*Read ADC channel 0 */
		r = ADC_read(0);
		r = (r-rmin)*rrange;
		_delay_us(150);
		
		/*Read ADC channel 1 */
		g = ADC_read(1);
		g = (g-gmin)*grange;
		_delay_us(150);
		
		/*Read ADC channel 2 */
		b = ADC_read(2);
		b = (b-bmin)*brange;
		
		/*set bounds for RGB values */
		if (r < 30)
			r = 0;
		else if (r > 255)
			r = 255;
		
		if (g < 30)
			g = 0;
		else if (g > 255)
			g = 255;
	
		if (b < 30)
			b = 0;
		else if (b > 255)
			b = 255;
			
		/*Write rgb values form the ADC */
		write_rgb(r, g, b);
		_delay_ms(5);
	}
	return 0; 
}

/**
* Initializes usart transmission
* @param ubrr myubrr
**/
void USART_Init(unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}
/**
* Sends usart transmission
* @param data character to transmit
**/
void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}/* Initializes ADC */void ADC_Init(void)
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	/*Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz */
	ADMUX |= (1 << REFS0); /*Set ADC reference */
	ADMUX |= (1 << ADLAR); /*Left adjust ADC result */
	ADCSRA |= (1 << ADEN);  /*Enable ADC */
}/**
* Reads a channel of the ADC
* @param ch channel of ADC to read
* @return ADCH value of conversion (high byte)
**/uint8_t ADC_read(uint8_t ch)
{
	/*Select ADC Channel */
	ADMUX &= 0xF0; /*Clear previous channel */
	ADMUX |= ch; /*Select channel mux */
	ADCSRA |= (1<<ADSC); /*Start conversion */
	while (!(ADCSRA & (1<<ADIF))); /*Wait for conversion */
	ADCSRA |= (1<<ADIF); /*Clear flag */
	return ADCH;
}/*** prints a constant string over usart* @param str a string**/void printUART(const char *str)
{
	unsigned char i = 0;
	while (str[i]) {
		USART_Transmit(str[i]);
		i++;
		_delay_ms(50);
	}
}
/**
* reads max and min values on each accelerometer axis
* @param rmax memory address of maximum r value
* @param gmax memory address of maximum g value
* @param bmax memory address of maximum b value
* @param rmin memory address of minimum r value
* @param gmin memory address of minimum g value
* @param bmin memory address of minimum b value
**/
void callibration(int *rmax, int *rmin,
int *gmax, int *gmin, int *bmax, int *bmin)
{
	int r = 0, g = 0, b = 0, i;
	
	/*Loops 1000 times recording the max & min values on each channel */
	for (i = 0; i < 1000; i++) {
		r = ADC_read(0);
		if (r > *rmax)
		*rmax = r;
		else if (r < *rmin)
		*rmin = r;

		_delay_us(1500);
		g = ADC_read(1);
		if (g > *gmax)
		*gmax = g;
		else if (g < *gmin)
		*gmin = g;
		
		_delay_us(1500);
		b = ADC_read(2);
		if (b > *bmax)
		*bmax = b;
		else if (b < *bmin)
		*bmin = b;
		
		_delay_us(1500);
	}
}