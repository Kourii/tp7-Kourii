/*********************************************************************************************************
 *                                               <Module name>
 *						<Module description>
 *
 *						<Copyright>
 *
 *						<Copyright or distribution terms>
 *
 *
 *********************************************************************************************************/

/*********************************************************************************************************
 *                                               <File description>
 *
 * Filename	: Filename
 * Version	: Module version
 * Programmer(s) : Programmer initial(s)
 **********************************************************************************************************
 *  Note(s):
 *
 *
 *
 *********************************************************************************************************/

/*********************************************************************************************************
 *
 * \file		${file_name}
 * \brief		Descripcion del modulo
 * \date		${date}
 * \author		Nicolas Ferragamo nferragamo@frba.utn.edu.ar
 * \version
 *********************************************************************************************************/

/*********************************************************************************************************
 *** INCLUDES
 *********************************************************************************************************/

#include <Arduino.h>
#include <util/delay.h>

/*********************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 *********************************************************************************************************/
#define SET_PIN(PORT, PIN) (PORT |= 1 << PIN)
#define CLEAR_PIN(PORT, PIN) (PORT &= ~(1 << PIN))

#define bt1 ((PINC >> PC0) & 0x01)
#define bt2 ((PINC >> PC1) & 0X01)

/*********************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 *********************************************************************************************************/
/**

Tabla de conversion bcd a 7 segmentos ánodo común.

  Codigo bcd		a	b	c	d	e	f	g   dp
  0				0	0	0	0	0	0	1   1
  1				1	0	0	1	1	1	1   1
  2				0	0	1	0	0	1	0   1
  3				0	0	0	0	1	1	0   1
  4				1	0	0	1	1	0	0   1
  5				1	0	1	1	0	1	1   1
  6				1	1	0	0	0	0	0   1
  7				0	0	0	1	1	1	1   1
  8				0	0	0	0	0	0	0   1
  9				0	0	0	1	1	0	0   1
*/
// uint8_t Tabla_Digitos_7seg[] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0xED, 0x83, 0xF8, 0x80, 0x98};

/**
Tabla de conversion bcd a 7 segmentos cátodo común
	Codigo bcd		a	b	c	d	e	f	g   dp
	0				      1	1	1	1	1	1	0   0
	1				      0	1	1	0	0	0	0
 	2				      1	1	0	1	1	0	1
 	3				      1	1	1	1	0	0	1
 	4				      0	1	1	0	0	1	1
	5				      1	0	1	1	0	1	1
	6				      0	0	1	1	1	1	1
	7				      1	1	1	0	0	0	0
 	8				      1	1	1	1	1	1	1
	9				      1	1	1	0	0	1	1
*/
uint8_t Tabla_Digitos_7seg[] = {
0b1111110,//0
0b0110000,//1
0b1101101,//2
0b1111001,
0b0110011,
0b1011011,//5
0b0011111,
0b1110000,
0b1111111,
0b1110011};

// static uint8_t Tabla_Digitos_BCD_7seg[ ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/*********************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 *   se escriben en CamelCase y están precedidas por la identificación del
 *   módulo seguida de un _
 *   ej: MEM_POOL  Mem_PoolHeap;
 *********************************************************************************************************/
uint8_t cont = 1;
uint32_t time = 100;
uint8_t flag_bt1 = 0;
uint8_t multiplier = 0;
uint8_t time_bt_t = 0;

/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *   se declaran con static y se escriben en CamelCase y están precedidas por la
 *   identificación del módulo seguida de un _ y usan _ para separar las palabras
 *   cuando tienen nombres compuestos
 *   ej: static MEM_POOL  Mem_Pool_Heap;
 *********************************************************************************************************/

/*********************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 * se declaran como estaticas y se escriben en CamelCase, están precedidas por la
 * identificación del módulo seguida de un _. No se decaran en el .h
 * ej static void Clk_DateTimer(parametros)
 *********************************************************************************************************/

/*********************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/
void config_TIMER0(void)
{
  TCCR0A = (1 << WGM01);              //Activa el bit CTC (clear timer on compare match)
                                      //del TCCR0A (timer counter/control register)
  OCR0A = 155;                        //valor de comparacion de int cada 10ms
  TCCR0B = (1 << CS00) | (1 << CS02); //preescaler dividiendo 1024
  TIMSK0 = (1 << OCIE0A);             //Habilita las interrupciones entimer compare
}
/*********************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 *********************************************************************************************************/

/**
  \fn  		Nombre de la Funcion
  \brief 		Descripcion
  \author 	Nombre
  \date 		${date}
  \param [in] 	parametros de entrada
  \param [out] 	parametros de salida
  \return tipo 	y descripcion de retorno
*/
ISR(TIMER0_COMPA_vect)
{
  multiplier++;
  if (multiplier > time)
  {
    PORTB ^= (1 << PORTB5);
    multiplier = 0;

    cont++;
    cont %= 16;
  }
  if (time_bt_t)
  {
    time_bt_t--;
  }
  
}
/*

Respetar este pin out.

PC0 -> BOTON 1
PC1 -> BOTON 2
PC2 -> BOTON 3
PC3 -> BOTON 4

PD2 -> SEGMENTO A
PD3 -> SEGMENTO B
PD4 -> SEGMENTO C
PD5 -> SEGMENTO D
PD6 -> SEGMENTO E
PD7 -> SEGMENTO F
PB0 -> SEGMENTO G
PB1 -> SEGMENTO DP

*/

/*
** The main function. Application starts here.
*/
int main(void){

  DDRC &=~(1<<PC0);
  DDRC &=~(1<<PC1);
  DDRC &=~(1<<PC2);
  DDRC &=~(1<<PC3);

  DDRD  |=(1<<PD2);
  DDRD  |=(1<<PD3);
  DDRD  |=(1<<PD4);
  DDRD  |=(1<<PD5);
  DDRD  |=(1<<PD6);
  DDRD  |=(1<<PD7);
  DDRB  |=(1<<PB0);
  DDRB  |=(1<<PB1);

  PORTD=Tabla_Digitos_7seg[5];
  PORTB=Tabla_Digitos_7seg[5];
}
/*********************************************************************************************************
** end of file
*********************************************************************************************************/