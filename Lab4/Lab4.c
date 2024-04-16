/*
 * Universidad del Valle de Guatemala
 * IE2023: Programación de Microcontroladores
 * Lab4.c
 * Autor: Diego Duarte 22426
 * Proyecto: Laboratorio 4
 * Hardware: ATMEGA328P
 * Creado: 09/04/2024
 * Última modificación: 09/04/2024
 */ 

//Librerias
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

uint16_t valor_contador = 0; 
uint8_t alternador_displays = 1;
uint8_t valor_pot = 0;
uint8_t tabla_7_seg[] = {0x80, 0xF2, 0x48, 0x60, 0x32, 0x24, 0x04, 0xF0, 0x00, 0x20, 0x10, 0x06, 0x8C, 0x42, 0x0C, 0x1C};
uint8_t prueba_pot = 0xFF;

void setup(void);

int main(void)
{
    setup();
    while (1) 
    {
		uint16_t valor_para_display = 0;
		uint8_t valor_portb = 0;
		uint8_t valor_portd = 0;
		uint8_t puntero = 0;
		if (alternador_displays == 0)
		{
			valor_para_display = valor_contador;
			valor_para_display <<= 2;
			valor_portd = ~(valor_portd | valor_para_display);
			valor_para_display >>= 8;
			valor_portb = (~(valor_portb | valor_para_display) & 0x03); // 0b0000_0011
			valor_portb = (PORTB & 0xFC) | valor_portb; // 0b1111_1100
			valor_portb &= 0xEF; // 0b1110_1111
			valor_portb |= 0x0C; // 0b0000_1100
			PORTD = valor_portd;
			PORTB = valor_portb;
		} else if (alternador_displays == 1) // Parte alta (display izquierdo)
		{
			puntero = (valor_pot >> 4) & 0x0F;
			//puntero = (prueba_pot >> 4) & 0x0F;
			valor_para_display = tabla_7_seg[puntero];
			valor_para_display <<=1;
			valor_portd = (valor_portd | valor_para_display);
			valor_para_display >>= 8;
			valor_portb = (valor_portb | valor_para_display) & 0x01; // 0b0000_0001
			valor_portb = (PORTB & 0xFE) | valor_portb; // 0b1111_1110
			valor_portb &= 0xF7; // 0b1111_0111
			valor_portb |= 0x16; // 0b0001_0110
			PORTD = valor_portd;
			PORTB = valor_portb;
		} 
		else // Parte baja (display derecho)
		{
			puntero = valor_pot & 0x0F;
			//puntero = prueba_pot & 0x0F;
			valor_para_display = tabla_7_seg[puntero];
			valor_para_display <<=1;
			valor_portd = (valor_portd | valor_para_display);
			valor_para_display >>= 8;
			valor_portb = (valor_portb | valor_para_display) & 0x01; // 0b0000_0001
			valor_portb = (PORTB & 0xFE) | valor_portb; // 0b1111_1110
			valor_portb &= 0xFB; // 0b1111_1011
			valor_portb |= 0x1A; // 0b0001_1010
			PORTD = valor_portd;
			PORTB = valor_portb;
		}
    }
}

void init_ADC(void){
	ADMUX = 0;
	// Vref = AVCC = 5V
	ADMUX |= (1<<REFS0);
	
	// Justificado a la izquierda
	ADMUX |= (1<<ADLAR);
	
	// Selección del canal A7
	ADMUX |= 0x07;
	
	ADCSRA = 0;
	
	// Habilitar ADC
	ADCSRA = (1<<ADEN);
	
	// Habilitar auto-trigger
	ADCSRA |= (1<<ADATE);
	
	// Máscara de la interrupción del ADC
	ADCSRA |= (1<<ADIE);
	
	// Prescaler del ADC a 128
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	// Iniciar primera conversión
	ADCSRA |= (1<<ADSC);
	
	ADCSRB = 0;
	
}

void setup(void){
	cli();
	DDRD |= 0xFC;
	PORTD |= 0xFC;
	DDRB |= 0x1F;
	PORTB |= 0x0F;
	DDRC = 0x00;
	PCMSK1 |= 0x03;
	PCICR |= (1 << PCIE1);
	
	TCCR0B |= (1 << CS00)|(1<<CS02);
	TCNT0 = 177;
	
	TIMSK0 = (1 << TOIE0);
	init_ADC();
	sei();
}

ISR(PCINT1_vect){
	if (!(PINC & (1 << PINC0))) // Decrementar
	{
		valor_contador -= 1;
		if (valor_contador == -1) {
			valor_contador = 255;
		}
	} 
	else if (!(PINC & (1 << PINC1))) // Incrementar
	{
		valor_contador += 1;
		if (valor_contador == 256) {
			valor_contador = 0;
		}
	}
}

ISR(TIMER0_OVF_vect){
	TCNT0 = 177;
	alternador_displays += 1;
	if (alternador_displays == 3)
	{
		alternador_displays = 0;
	}
}

ISR(ADC_vect){
	valor_pot = ADCH;
}
