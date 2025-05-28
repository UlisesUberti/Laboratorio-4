/*********************************************************************************************************************
Copyright (c) 2025, Uberti,Ulises Leandro <ubertileandro0@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file screen.c
 * @brief Código fuente de screen.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "screen.h"
#include "poncho.h"
/* === Macros definitions ========================================================================================== */
#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

// LO MISMO CON EL RESTO DE SEGMENTOS --> ASIGNANDOLE A CADA SEGNMENTO EL BIT CORRESPONDIENTE AL HARDWARE

/* === Private data type declarations ============================================================================== */

// defino el mapeo de los segmentos segun el numero
// Digit_Map es una lista con 9 componentes cada uno correspondiente a los segmentos que deben prenderse segun el numero
// a mostrar
// | es el operador OR binario
static const uint8_t Digit_Map[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_G | SEGMENT_E | SEGMENT_D,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_G | SEGMENT_F,                                     // 4
    SEGMENT_A | SEGMENT_F | SEGMENT_G | SEGMENT_C | SEGMENT_D,                         // 5
    SEGMENT_A | SEGMENT_F | SEGMENT_G | SEGMENT_E | SEGMENT_C | SEGMENT_D,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};
// defino la estrucutura del objeto pantalla
struct screen_s {
    uint8_t digits;                   // cantidad de digitos de la pantalla (nuestro caso 4)
    uint8_t value[SCREEN_MAX_DIGITS]; // value es un arreglo que almacena que numero debe estar en cada digito
    uint8_t current_digit;            // el digito actual activado durante el multiplexado
    screen_driver_t driver;           // estructura de punteros a funciones (apagar,prender,actualizar segmentos)
    // esta estrucutra permite que screen no depende directamente del hardware
};

/* === Private function declarations =============================================================================== */

// Funcion Definicion de los digitos
// Funcion que permite definir los pines GPIO del enable de cada display
void Digits_Init(void);

// funcion para la definicion de los segmentos
// Funcion que define los segmentos de un display del A al G
void Segments_Init(void);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t Screen_Create(screen_driver_t driver, uint8_t digits) {
    // el argumento driver debe definirse en el programa principal
    // asigno espacio en memoria para la estrucutura del objeto pantalla
    screen_t screen = malloc(sizeof(struct screen_s));
    if (digits > SCREEN_MAX_DIGITS) {
        // caso en el que se intente poner mas digitos qu ee ancho de la pantalla
        digits = SCREEN_MAX_DIGITS;
    }
    if (screen != NULL) {
        // me aseguro que le asigne una direc al puntero
        screen->digits = digits;
        // le asigno al campo de digitos de la pantalla la cantidad de digitos
        // agrego las funciones del fabricante que definen a los digitos --> SCU-pinmuxset
        //  CHIP_GPIO_ Setpinstate ... en false para apagar los digitos
        //  se agrega la funcion clear value tmb
        screen->driver = driver;
        screen->current_digit = 0;
        // con als funciones internas llamo directamente a digits init y digits segment
    }
    return screen;
}
// Funcion que convierte de BCD a 7 segmentos y guarda en memoria de video
void Screen_Write_BCD(screen_t screen, uint8_t value[], uint8_t size) {
    // menset --> funcion de la libreria stdlib permite completar el array con 0
    memset(screen->value, 0, sizeof(screen->value));
    // Limpia la memoria de video (value[]) para poder escribir sin problemas
    //  value trae numeros del 0 al 9
    //  la memoria de video debera guardar los segmentos a escribir
    //  CONDICIONAL POR EL TAMAÑO
    //  size es la cantidad de digitos a mostrar
    //  value [] contiene el arreglo de numeros a escribir
    if (size > screen->digits) {
        size = screen->digits; // le asigno el tamaño maximo como la cantidad de digitos a escribir
    }
    for (uint8_t i = 0; i < size; i++) {
        screen->value[i] = Digit_Map[value[i]];
        // si i = 1 --> recorre el mapa hasta la posicion 1 y lo almacena en screen
        // ahora value[] es de la forma value[Digit_Map[i],...,Digit_Map[size]]
    }
}

// Funcion para el multiplexado
void Screen_Refresh(screen_t screen) {

    // ApagarDigitos(); // enable del display (los digitos son los enables del display)

    screen->driver.Digit_Turn_Off();                                      // Apago todos los digitos
    screen->current_digit = (screen->current_digit + 1) % screen->digits; // posiciona al objeto en el siguiente digito
    screen->driver.Segments_Turn_Update(screen->value[screen->current_digit]); // actualiza los segmentos
    screen->driver.Digit_Turn_On(screen->current_digit);                       // enciende el digito actual

    // screen->value[numeros a mostrar] si tengo 4 displays --> screen->value [0,0,1,2]
    //  PrenderDigit(screen->current_digit); // podria pasarle un argumento que provoque que se apaguen todos
    //  Digit_Turn_On(screen->current_digit);
    //  estas funciones deberian estar en la interface
}

/* === End of documentation ======================================================================================== */
