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

/** @file bsp.c
 * @brief Código fuente de bsp.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "chip.h"
#include "bsp.h"
#include "EDU-CIAA.h"

/* === Macros definitions ========================================================================================== */

// Aqui se ubican los define del main

/* === Private data type declarations ============================================================================== */
// static Board_t Board;
/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */
// static Board_t Board;
/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */
Board_t Board_Create() {
    // necesito una variable con 8 direcciones de memoria para crear 8 objetos
    // son los 4 leds y las 4 teclas
    // un struct necesitara almacenar esas 8 diferencias
    // es para prohibir que se cambie el valor de cada objeto en sus campos

    struct Board_s * Board = malloc(sizeof(struct Board_s));
    if (Board != NULL) {

        // Creo las salidas digitales
        Board->Led_Red = Digital_Out_Create(LED_1_GPIO, LED_1_BIT);
        Board->Led_B = Digital_Out_Create(LED_B_GPIO, LED_B_BIT);
        Board->Led_Yellow = Digital_Out_Create(LED_2_GPIO, LED_2_BIT);
        Board->Led_Green = Digital_Out_Create(LED_3_GPIO, LED_3_BIT);

        // Creo las entradas digitales
        Board->Sw1 = Digital_In_Create(TEC_1_GPIO, TEC_1_BIT, false);
        Board->Sw2 = Digital_In_Create(TEC_2_GPIO, TEC_2_BIT, false);
        Board->Sw3 = Digital_In_Create(TEC_3_GPIO, TEC_3_BIT, false);
        Board->Sw4 = Digital_In_Create(TEC_4_GPIO, TEC_4_BIT, false);
    }

    // Retorno el puntero a la estructura que almacena los objetos creados
    return Board;
}

/* === Public function implementation ============================================================================== */

/* === End of documentation ======================================================================================== */
