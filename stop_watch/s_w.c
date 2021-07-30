/*
 * s_w.c
 *
 *  Created on: May 4, 2021
 *      Author: sondos ahmed
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



#define ONE_SEC  975.5625//exact value to count one second
unsigned char reset=0,SEC=0,MIN=0,HOUR=0;



void INT0_Init(void)
{
	DDRD  &= (~(1<<PD2));
	PORTD|=(1<<PD2);
	GICR  |= (1<<INT0);
	MCUCR |= (1<<ISC01); //falling edge
	SREG|=(1<<7);
}


void INT1_Init(void)
{
	DDRD  &= (~(1<<PD3));
	GICR  |= (1<<INT1);
	MCUCR |= (1<<ISC11) | (1<<ISC10) ;//rising edge
	SREG  |=(1<<7);
}


void INT2_Init(void)
{
	DDRB  &= (~(1<<PB2));
	PORTB|=(1<<PB2);
	GICR  |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2);//falling edge
	SREG|=(1<<7);
}



void Timer1_init_compare_Mode(void)
{
	TCNT1= 0;
	OCR1A= ONE_SEC;
	TIMSK= (1<<OCIE1A);
	TCCR1A= (1<<FOC1A);
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12);// prescaler 1024
}



//*****************timer ISR********************//
ISR(TIMER1_COMPA_vect)
{
	SREG|=(1<<7);

	if(!reset){


		if(SEC==59){SEC=0;MIN++;}else{SEC++;}//1 minute

		if(MIN==59){MIN=0;HOUR++;}//1 hour


		if(HOUR==24){HOUR=0;reset=1;}// 1day


	}
	else{reset=0;}


}



//**********reset******************//

ISR(INT0_vect)
{
	reset=1;
	PORTC=0;
	SEC=0;MIN=0;HOUR=0;//reset timer
	//TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12);// enable timer1 (uncomment this line to reset and start counting without pressing resume )
}

//**********pause*****************//

ISR(INT1_vect)
{
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);// disable timer1
}

//**********resume******************//
ISR(INT2_vect)
{
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12);//enable timer1
}




int main(){
	DDRA  =0xFF; //PORTA O/P
	DDRC |=0x0F; //PORTC O/P
	PORTC&=0xF0; //PORTC initial condition
	SREG |=(1<<7);
	INT0_Init();
	INT1_Init();
	INT2_Init();
	Timer1_init_compare_Mode();

	while(1){

		PORTA = (1<<0);
		PORTC = SEC % 10;//SEC1
		_delay_us(5);
		PORTA = (1<<1);
		PORTC = SEC / 10;//SEC2
		_delay_us(5);
		PORTA = (1<<2);
		PORTC = MIN % 10;//MIN1
		_delay_us(5);
		PORTA = (1<<3);
		PORTC = MIN / 10;//MIN2
		_delay_us(5);
		PORTA = (1<<4);
		PORTC = HOUR % 10;//HPUR1
		_delay_us(5);
		PORTA = (1<<5);
		PORTC = HOUR / 10;//HOUR2
		_delay_us(5);
	}
}
