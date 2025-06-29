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

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Codigo fuente de bsp.h
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "chip.h"
#include "screen.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estrutura del objeto placa, displays, teclas y buzzer
 *
 */
typedef struct Board_s {
    /* Estructura con los atributos de la pantalla*/

    // soporte de la salida digital (zumbador)
    Digital_Out_t Buzzer; // Zumbador de la alarma

    // soporte de 6 entradas digitales (teclas)
    Digital_In_t Set_Time;  // Entrada de seteo de la hora
    Digital_In_t Set_Alarm; // Entrada de seteo de la alarma
    Digital_In_t Decrement; // Disminucion de valor en display
    Digital_In_t Increment; // Aumento de valor en display
    Digital_In_t Accept;    // Entrada de seleccion (Aceptar)
    Digital_In_t Cancel;    // Entrada de seleccion (Cancelar)

    // Soporte para display de 7 segmentos
    screen_t Screen;

    // Leds de la placa (inferiores)
    Digital_Out_t Led_1;
    Digital_Out_t Led_2;
    Digital_Out_t Led_3;
    Digital_Out_t Led_R;
    Digital_Out_t Led_G;
    Digital_Out_t Led_B;

} const * Board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear el objeto
 *
 * @return Board_t puntero al objeto
 */
Board_t Board_Create(void);

/**
 * @brief Funcion que devuelve la cantidad de milisegundos desde que arranco el sistema
 *
 * @return uint32_t
 */
uint32_t Board_getMillis(void);

/**
 * @brief Funcion para inicializar el contador de tiempo
 *
 */
void Init_Tick(void);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */
