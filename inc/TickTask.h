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

#ifndef TICKTASK_H_
#define TICKTASK_H_

/** @file TickTask.h
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
#define TICK_1_SECOND_EVENT (1 << 8)

// Cantidad minima de stack asignada a la tarea
#define Tick_Task_Stack_Size (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

// Declaramos un struct para pasarle luego los argumentos a la tarea
typedef struct Tick_Task_Args_s {
    EventGroupHandle_t clock_events; // grupo de eventos
    int tick_event;
} * Tick_Task_Args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// Declaramos las funciones de la tarea, estructura: void nameTask(void*);

/**
 * @brief Funcion Tick
 * Se encarga de generar un evento cada 1 seg con precision y avisarle a Clock para que avance
 * @param args
 */
void Tick_Task(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* TICKTASK_H_ */
