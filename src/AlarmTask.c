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
#include "AlarmTask.h"
#include "ClockTask.h"
#include "ButtonTask.h"
#include <stdio.h>
#include "chip.h"
#include <stdbool.h>
#include "bsp.h"
#include "clock.h"
#include "DigitalOut.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void Alarm_Task(void * args) {
    Alarm_Task_Args_t parameters = args;
    EventBits_t events;
    parameters->current_time = Clock_Time(parameters->clock);
    while (true) {
        events = xEventGroupWaitBits(parameters->clock_events,
                                     ALARM_DEACTIVATE_EVENT | ALARM_OFF_EVENT | ALARM_ON_EVENT | ALARM_SNOOZE_EVENT |
                                         ALARM_TIME_EVENT,
                                     pdTRUE, pdFALSE, portMAX_DELAY);

        if ((events & ALARM_TIME_EVENT)) {
            // Prende led indicando alarma activa
            Digital_Out_Activate(parameters->Board->Led_3);
        } else if ((events & ALARM_ON_EVENT) && (events & ALARM_SNOOZE_EVENT)) {
            // Apaga el led de la alarma activa
            Digital_Out_Deactivate(parameters->Board->Led_3);
        } else if ((events & ALARM_OFF_EVENT)) {
            // Aapaga el led de la alarma activa
            Digital_Out_Deactivate(parameters->Board->Led_3);
        }
    }
}
/* === End of documentation ======================================================================================== */
