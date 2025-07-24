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

/** @file TickTask.c
 * @brief Código fuente de TickTask.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include "TickTask.h"
#include "ButtonTask.h"
#include <stdio.h>
#include "chip.h"
#include <stdbool.h>
#include "bsp.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void Tick_Task(void * args) {
    Tick_Task_Args_t parameters = args;
    // Declaro una variable del tipo TickType
    TickType_t last_time;
    // La incializo con la funcion de FreeRTOS para contar Ticks
    last_time = xTaskGetTickCount();
    // Declaro una variable que representa el tiempo total
    TickType_t duration = pdMS_TO_TICKS(1000);

    while (true) {
        // Utilizo una funcion de FreeRTOS para que la tarea se bloquee un segundo con periodo fijo
        vTaskDelayUntil(last_time, duration);
        // Una vez que paso 1 seg se dispara el evento
        xEventGroupSetBits(parameters->clock_events, TICK_1_SECOND_EVENT);
    }
}
/* === End of documentation ======================================================================================== */
