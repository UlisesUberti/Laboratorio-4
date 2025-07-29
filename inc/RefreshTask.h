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

#ifndef REFRESHTASK_H_
#define REFRESHTASK_H_

/** @file RefreshTask.h
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

// Cantidad minima de stack asignada a la tarea
#define Refresh_Task_Stack_Size (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

// Declaramos un struct para pasarle luego los argumentos a la tarea
typedef struct Refresh_Task_Args_s {
    Board_t Board;                   // puntero al objeto placa
    EventGroupHandle_t clock_events; // grupo de eventos
    SemaphoreHandle_t screen_Mutex;  // Handle al mutex de la pantalla
} * Refresh_Task_Args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// Declaramos la funcione de la tarea, estructura: void nameTask(void*);

/**
 * @brief Tarea Refresco
 * Se encarga de refrescar la pantalla cada 1[ms]
 * @param args
 */
void Refresh_Task(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* REFRESHTASK_H_ */
