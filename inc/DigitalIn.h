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

#ifndef DIGITALIN_H_
#define DIGITALIN_H_

/** @file DigitalIn.h
 ** @brief
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Digital_In_t es un puntero a la estructura Digital_In_s
 *
 */
typedef struct Digital_In_s * Digital_In_t;

/**
 * @brief Digital_States_e es un enum con 3 estados posibles
 *
 */
typedef enum Digital_States_e {
    Input_Was_Deactiveted = -1,
    Input_Was_Activeted = 1,
    Input_NOT_CHANGE = 0,
} Digital_States_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear una Entrada Digital como un objeto
 *
 * @param port Puerto GPIO
 * @param bit Bit del Puerto
 * @param inverted Logica (Invertida o Directa)
 * @return Digital_In_t - Retorna un puntero a la direccion de memoria donde se almacenara
 */
Digital_In_t Digital_In_Create(uint8_t port, uint8_t bit, bool inverted);

/**
 * @brief Funcion para obtener el estado de la entrada digital
 *
 * @return true
 * @return false
 */
bool Digital_In_GetState(Digital_In_t Digital_In);

/**
 * @brief Funcion para determinar si una entrada digital estaba activa
 *
 * @return true
 * @return false
 */
bool Digital_In_Was_Activated(Digital_In_t Digital_In);

/**
 * @brief Funcion para determinar si el estado anterior de una señal fue desactivado
 *
 * @return true
 * @return false
 */
bool Digital_In_Was_Deactivated(Digital_In_t Digital_In);

/**
 * @brief Funcion para determinar si cambio el estado de la señal
 *
 * @return true
 * @return false
 */
Digital_States_t Digital_In_Was_Changed(Digital_In_t Digital_In);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITALIN_H_ */
