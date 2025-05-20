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

#ifndef DIGITALOUT_H_
#define DIGITALOUT_H_

/** @file DigitalOut.h
 ** @brief Codigo Digital Out.h
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
 * @brief Struct Digital_Out_s y puntero a la estructura Digital_Out_t
 */
typedef struct Digital_Out_s * Digital_Out_t;

/* === Public variable declarations ================================================================================ */

/**
 * @brief Funcion para crear una salida digital
 *
 * @param port Puerto fisico
 * @param bit pin fisico
 * @return Digital_Out_t
 */
Digital_Out_t Digital_Out_Create(uint8_t port, uint8_t bit);

/**
 * @brief Funcion para activar la salida digital
 * No retorna nada, solo activa la salida
 * @param Digital_Out Puntero a la estructura con los parametros de la salida
 */
void Digital_Out_Activate(Digital_Out_t Digital_Out);

/**
 * @brief Funcion para desactivar la salida digital
 * No retorna nada, solo desactiva la salida
 * @param Digital_Out Puntero a la estructura con los parametros de la salida
 */
void Digital_Out_Deactivate(Digital_Out_t Digital_Out);

/**
 * @brief Funcion para cambiar el estado logico de una salida digital
 *
 * @param Digital_Out Puntero a la estructura con los parametros de la salida
 */
void Digital_Out_Toggle(Digital_Out_t Digital_Out);

/* === Public function declarations ================================================================================ */

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITALOUT_H_ */
