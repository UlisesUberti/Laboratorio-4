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

/** @file ClockTask.c
 * @brief Código fuente de ClockTask.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include "ClockTask.h"
#include "ButtonTask.h"
#include <stdio.h>
#include "chip.h"
#include <stdbool.h>
#include "clock.h"
#include "TickTask.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

typedef enum {
    Clock_Init_Mode,
    Clock_Time_Mode,
    Clock_Set_Minutes_Mode,
    Clock_Set_Hours_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Set_Minutes_Alarm_Mode,
    Clock_Set_Hours_Alarm_Mode,
} Clock_Mode_t;

static Clock_Mode_t actual_mode;

/* === Private function declarations =============================================================================== */

void Clock_Task(void * args) {
    // Defino un puntero a la estrucutra con los argumentos de la tarea segun el parametro que se paso
    Clock_Task_Args_t param = args;
    // Defino eventos del tipo EventBits_t para esperar los eventos
    EventBits_t events;
    // loop infinito
    while (true) {
        // Espero que ocurra un evento
        events = xEventGroupWaitBits(param->clock_Events,
                                     SW_0_EVENT | SW_1_EVENT | SW_2_EVENT | SW_3_EVENT | SW_4_EVENT | SW_5_EVENT |
                                         SW_6_EVENT | SW_LONG_DURATION_EVENT | TICK_1_SECOND_EVENT,
                                     pdTRUE, pdFALSE, portMAX_DELAY);
        // portMAX_DELAY indica que esperara todo el tiempo
        // pdTRUE indica que limpiara los bits una vez recibido el evento
        // pdFALSE indica que no esperara a que todos los eventos hayan ocurrido

        // Avanzo 1 seg
        if (events & TICK_1_SECOND_EVENT) {
            Clock_New_Tick(param->clock);
        }

        // Verifico que el mutex no esta ocupado
        // Condicional con Mutex para verificar que la pantalla no esta siendo refrescada
        // portMax_Delay indica que esperara todo el tiempo necesario hasta que se libere
        if (xSemaphoreTake(param->screen_Mutex, portMAX_DELAY)) {
            switch (actual_mode) {
            case Clock_Init_Mode:
                Display_Flash_Digits(param->Board->Screen, 0, 3, 200);
                Flash_Point(param->Board->Screen, 1, 1, 200);

                break;
            case Clock_Set_Minutes_Mode:
                Display_Flash_Digits(param->Board->Screen, 2, 3, 170);
                Flash_Point(param->Board->Screen, 1, 1, 170);

                break;
            case Clock_Set_Hours_Mode:
                Display_Flash_Digits(param->Board->Screen, 0, 1, 170);
                Flash_Point(param->Board->Screen, 1, 1, 170);

                break;
            case Clock_Time_Mode:
                Display_Flash_Digits(param->Board->Screen, 0, 4, 0);
                Flash_Point(param->Board->Screen, 1, 1, 1000);

                break;
            case Clock_Set_Alarm_Mode:
                Display_Flash_Digits(param->Board->Screen, 0, 3, 0);
                All_Points_On(param->Board->Screen);

                break;
            case Clock_Set_Minutes_Alarm_Mode:
                Display_Flash_Digits(param->Board->Screen, 2, 3, 170);
                Flash_Point(param->Board->Screen, 1, 1, 170);

                break;
            case Clock_Set_Hours_Alarm_Mode:
                Display_Flash_Digits(param->Board->Screen, 0, 1, 170);
                Flash_Point(param->Board->Screen, 1, 1, 170);

                break;

            default:
                break;
            }
            // libero el mutex
            xSemaphoreGive(param->screen_Mutex);
        }

        if (actual_mode == Clock_Init_Mode) {
            if ((events & SW_2_EVENT) && (events & SW_LONG_DURATION_EVENT)) {
                // cambia el estado
                actual_mode = Clock_Set_Minutes_Mode;
            }
        } else if (actual_mode == Clock_Set_Minutes_Mode) {
            if (events & SW_0_EVENT) {
                // cambia el estado
                actual_mode = Clock_Set_Hours_Mode;
            } else if (events & SW_1_EVENT) {
                // Cambia el estado
                actual_mode = Clock_Time_Mode;
            } else if (events & SW_5_EVENT) {
                // Incrementa minutos
            } else if (events & SW_4_EVENT) {
                // Decrementa minutos
            }
        } else if (actual_mode == Clock_Set_Hours_Mode) {
            if (events & SW_0_EVENT) {
                // Cambia el estado
                actual_mode = Clock_Time_Mode;
            } else if (events & SW_1_EVENT) {
                // Cambia el estado
                actual_mode = Clock_Set_Minutes_Mode;
            } else if (events & SW_5_EVENT) {
                // Incrementa horas
            } else if (events & SW_4_EVENT) {
                // Decrementa horas
            }
        } else if (actual_mode == Clock_Time_Mode) {
            if (events & SW_0_EVENT) {
                // activa alarma
            } else if (events & SW_1_EVENT) {
                // desactiva alarma
            } else if (events & SW_3_EVENT) {
                // Cambia el estado
                actual_mode = Clock_Set_Alarm_Mode;
            } else if ((events & SW_2_EVENT) && (events & SW_LONG_DURATION_EVENT)) {
                // Cambia el estado
                actual_mode = Clock_Set_Minutes_Mode;
            }
        } else if (actual_mode == Clock_Set_Alarm_Mode) {
            if ((events & SW_2_EVENT) && (events & SW_LONG_DURATION_EVENT)) {
                // Cambia el estado
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
            } else if (events & SW_1_EVENT) {
                // cambia el estado
                actual_mode = Clock_Time_Mode;
            }
        } else if (actual_mode == Clock_Set_Minutes_Alarm_Mode) {
            if (events & SW_0_EVENT) {
                // cambia el estado
                actual_mode = Clock_Set_Hours_Alarm_Mode;
            } else if (events & SW_1_EVENT) {
                // cambia el estado
                actual_mode = Clock_Time_Mode;
            } else if (events & SW_5_EVENT) {
                // Incrementar minutos
            } else if (events & SW_4_EVENT) {
                // Decrementar minutos
            }
        } else if (actual_mode == Clock_Set_Hours_Alarm_Mode) {
            if (events & SW_0_EVENT) {
                // cambia el estado
                actual_mode = Clock_Time_Mode;
            } else if (events & SW_1_EVENT) {
                // cambia el estado
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
            }
        }
    }
}
/* === Private variable definitions* ================================================================================*/

/* === Public variable definitions* =================================================================================*/

/* === Private function definitions* ================================================================================*/

/* === Public function implementation* ==============================================================================*/

/* === End of documentation* ========================================================================================*/
