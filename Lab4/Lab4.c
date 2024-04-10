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

void setup(void);


int main(void)
{
    setup();
    while (1) 
    {
		uint16_t valor_para_display = valor_contador;
		uint8_t valor_portb = 0;
		uint8_t valor_portd = 0;
		valor_para_display <<= 2;
		valor_portd = ~(valor_portd | valor_para_display);
		valor_para_display >>= 8;
		valor_portb = (~(valor_portb | valor_para_display) & 0x03); // 0b0000_0011
		valor_portb = (PORTB & 0xFC)| valor_portb;
		PORTD = valor_portd;
		PORTB = valor_portb;
    }
}

void setup(void){
	DDRD |= 0xFC;
	PORTD |= 0xFC;
	DDRB |= 0x1F;
	PORTB |= 0x0F;
	DDRC = 0x00;
	PCMSK1 |= 0x03;
	PCICR |= (1 << PCIE1);
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
