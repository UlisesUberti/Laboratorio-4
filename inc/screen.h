
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

#ifndef EDU_CIAA_H_
#define EDU_CIAA_H_

/** @file .h
 ** @brief Codigo fuente de
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include "DigitalIn.h"
#include "DigitalOut.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
// creo la etructura correspondiente al objeto pantalla y un puntero a la misma
typedef struct screen_s * screen_t; //{
//  Digit_Turn_On_t DigitTurnOff;
//};

// funciones callback

typedef void(*Digits_Turn_Off_t(void));
typedef void(*Digit_Turn_On_t(uint8_t));

typedef void(*Segments_Turn_Update_t(uint8_t));

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// La funcione que crea la pantalla, con los parametros que necesita
screen_t Screen_Create(uint8_t digits); // devuelve un puntero a screen_s

// funcion que no retnorna nada pero escribe en pantalla, recibe la pantalla
void Screan_Write_BCD(screen_t screen, uint8_t value[], uint8_t size);

// Una funcion de refresco
void Screen_Refresh(screen_t screen);

// funcion para el parpadeo
void Display_Flash_Digits(screen_t display, uint8_t from, uint8_t to, uint8_t frecuency);
/* === Public function declarations ================================================================================ */

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* EDU_CIAA_H_ */
