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

#ifndef CLOCKTASK_H_
#define CLOCKTASK_H_

/** @file ClockTask.h
 ** @brief
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "clock.h"
#include "bsp.h"
#include "task.h"
#include "semphr.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

// Cantidad de displays
#define CANT_DISPLAYS 4

// Cantidad minima de stack asignada a la tarea
#define Clock_Task_Stack_Size (3 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

// Struct para pasarle como argumento a la tarea con los parametros necesarios
typedef struct Clock_Task_Args_s {
    clock_t clock;                   // Puntero al objeto reloj
    clock_time_t current_time;       // Puntero a la hora del reloj
    clock_time_t alarm_time;         // Puntero a la alarma del reloj
    Board_t Board;                   // Puntero al objeto placa
    uint8_t tick;                    // Evento de tick
    EventGroupHandle_t clock_Events; // Handle al grupo de eventos
    SemaphoreHandle_t screen_Mutex;  // Handle al mutex de la pantalla

} * Clock_Task_Args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// Declaramos las funciones de la tarea, estructura: void nameTask(void*);

/**
 * @brief Tarea Reloj
 * Se encarga de avanzar cada 1 seg la hora y manejar los modos
 * avisa si es momento de la alarma mediante un evento
 * @param args
 */
void Clock_Task(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITALIN_H_ */
