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

#ifndef BUTTONTASK_H_
#define BUTTONTASK_H_

/** @file ButtonTask.h
 ** @brief
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "clock.h"
#include "bsp.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

// Realizo una mascara de bits correspondiente a los eventos

#define SW_0_EVENT (1 << 0)      // Evento boton 0
#define SW_1_EVENT (1 << 1)      // Evento boton 1
#define SW_2_EVENT (1 << 2)      // Evento boton 2
#define SW_3_EVENT (1 << 3)      // Evento boton 3
#define SW_4_EVENT (1 << 4)      // Evento boton 4
#define SW_5_EVENT (1 << 5)      // Evento boton 5
#define SW_LONG_3_EVENT (1 << 6) // Evento Presionado durante x tiempo el boton
#define SW_LONG_EVENT (1 << 7)   // Evento Presionado durante x tiempo el boton

// Cantidad minima de stack asignada a la tarea
#define Button_Task_Stack_Size (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

// Declaramos un struct para pasarle luego los argumentos a la tarea

typedef struct Button_Task_Args_s {
    Board_t Board;                   // Objeto placa
    EventGroupHandle_t clock_events; // grupo de eventos
    uint8_t event_short_bit;         // Bits asociados a evento de presionar un boton
    uint8_t event_long_bit;          // evento asociado presionar un boton durante x tiempo
    Digital_In_t Switch;             // Entrada digital
} * Button_Task_Args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// Declaramos las funciones de la tarea, estructura: void nameTask(void*);

/**
 * @brief Tarea del boton
 * Tarea que se encarga de avisar mediante un evento cuando un boton fue presionado
 * de corta duracion
 * @param args puntero a los parametros a pasarle comoa argumento
 */
void Button_Short_Task(void * args);

/**
 * @brief Tarea del boton
 * Tarea que se encarga de avisar mediante un evento cuando un boton fue presionado
 * de larga duracion
 * @param args puntero a los parametros a pasarle comoa argumento
 */
void Button_Long_Task(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BUTTONTASK_H_ */
