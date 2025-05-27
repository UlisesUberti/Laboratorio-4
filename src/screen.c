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

/** @file DigitalOut.c
 * @brief Código fuente de DigitalOut
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "screen.h"
/* === Macros definitions ========================================================================================== */
#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

#define SEGMENT_A (1 << 0)
// LO MISMO CON EL RESTO DE SEGMENTOS --> ASIGNANDOLE A CADA SEGNMENTO EL BIT CORRESPONDIENTE AL HARDWARE

/* === Private data type declarations ============================================================================== */
// defino la estrucutura de la pantalla
struct screen_s {
    uint8_t digits; // cantidad de digitos
    uint8_t value[SCREEN_MAX_DIGITS];
    uint8_t current_digit;
};

/* === Private function declarations =============================================================================== */
// FUNCION DE MAPEO DE DIGITOS CON SEGMENTOS
static const uint8_t DIGIT_MAP[10] = {};
/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

void Digits_Init() {
    // funciones de fabricante
}
void Digits_Segments() {
    // funciones de fabricante
}

/* === Public function implementation ============================================================================== */

screen_t Screen_Create(uint8_t digits) {
    screen_t screen = malloc(sizeof(struct screen_s));
    if (digits > SCREEN_MAX_DIGITS) {
        // caso en el que se intente poner mas digitos qu ee ancho de la pantalla
        digits = SCREEN_MAX_DIGITS;
    }
    if (screen != NULL) {
        // me aseguro que le asigne una direc al puntero
        screen->digits = digits;

        // agrego las funciones del fabricante que definen a los digitos --> SCU-pinmuxset
        //  CHIP_GPIO_ Setpinstate ... en false para apagar los digitos
        //  se agrega la funcion clear value tmb

        // con als funciones internas llamo directamente a digits init y digits segment
    }
}

void Screan_Write_BCD(screen_t screen, uint8_t value[], uint8_t size) {
    // menset --> funcion de la libreria stdlib permite completar el array con 0

    // CONDICIONAL POR EL TAMAÑO

    // Bucle para asignar el digito a escribir segmento por sengmento
}

void Screen_Refresh(screen_t screen) {
    // ApagarDigitos(); // enable del display (los digitos osn los enables del display)
    Digits_Turn_Off();
    screen->current_digit = (screen->current_digit + 1) % screen->digits;
    // PrenderSegmentos(screen->value[screen->current_digit]);
    Segments_Turn_Update(screen->value[screen->current_digit]);
    // PrenderDigit(screen->current_digit); // podria pasarle un argumento que provoque que se apaguen todos
    Digit_Turn_On(screen->current_digit);
    // estas funciones deberian estar en la interface
}

/* === End of documentation ======================================================================================== */
