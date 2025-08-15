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

static Clock_Mode_t actual_mode;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions* ================================================================================*/

/* === Public variable definitions* =================================================================================*/

/* === Private function definitions* ================================================================================*/

/* === Public function implementation* ==============================================================================*/

void Clock_Task(void * args) {
    // Defino un puntero a la estrucutra con los argumentos de la tarea segun el parametro que se paso
    Clock_Task_Args_t param = args;
    // Defino eventos del tipo EventBits_t para esperar los eventos
    EventBits_t events;
    // Variable para llevar la hora del reloj
    clock_time_t actual_time = {0};
    // variable para llevar la hora de la alarma
    clock_time_t alarm_time = {0};
    // variable para llevar la hora de la alarma con delay
    clock_time_t alarm_with_delay = {0};
    // Seteamos un horario de alarma por defecto
    Clock_Set_Time_Alarm(param->clock, &alarm_time);
    // Asignamos al parametro de la tarea la hora designada por defecto
    param->alarm_time = Clock_Alarm(param->clock);
    // Defino un arreglo con el valor inicial de la hora del reloj
    uint8_t value[4] = {0};
    // Seteo el reloj en un horario inicial por defecto
    Clock_Set_Time(param->clock, &actual_time);
    Screen_Write_BCD(param->Board->Screen, value, 4);
    // Defino un modo incial del reloj
    Change_Mode(param->clock, Clock_Init_Mode, param->Board->Screen);
    // Bandera para la alarma
    bool alarm_sounding = false;
    // Contador para detectar inactividad
    TickType_t last_iteraction = xTaskGetTickCount();
    // loop infinito
    while (true) {
        xEventGroupClearBits(param->clock_Events, SW_0_EVENT | SW_1_EVENT | SW_2_EVENT | SW_3_EVENT | SW_4_EVENT |
                                                      SW_5_EVENT | SW_LONG_3_EVENT | TICK_1_SECOND_EVENT |
                                                      SW_LONG_EVENT);
        // Espero que ocurra un evento
        events = xEventGroupWaitBits(param->clock_Events,
                                     SW_0_EVENT | SW_1_EVENT | SW_2_EVENT | SW_3_EVENT | SW_4_EVENT | SW_5_EVENT |
                                         SW_LONG_EVENT | SW_LONG_3_EVENT | TICK_1_SECOND_EVENT,
                                     pdTRUE, pdFALSE, pdMS_TO_TICKS(150));
        // portMAX_DELAY indica que esperara todo el tiempo necesario un evento para ejecutar la tarea
        // pdTRUE indica que limpiara los bits una vez recibido el evento
        // pdFALSE indica que no esperara a que todos los eventos hayan ocurrido
        actual_mode = Clock_Mode(param->clock);
        if (actual_mode == Clock_Init_Mode) {
            if ((events & SW_LONG_EVENT)) {
                // cambia el estado
                Change_Mode(param->clock, Clock_Set_Minutes_Mode, param->Board->Screen);
            }
        } else if (actual_mode == Clock_Set_Minutes_Mode) {
            if (events & SW_0_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Hours_Mode, param->Board->Screen);
            } else if (events & SW_1_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Time_Mode, param->Board->Screen);
            } else if (events & SW_5_EVENT) {
                // incrementa los minutos
                Clock_Increment_Minutes(&actual_time);
            } else if (events & SW_4_EVENT) {
                // decrementa los minutos
                Clock_Decrement_Minutes(&actual_time);
            }
        } else if (actual_mode == Clock_Set_Hours_Mode) {
            if (events & SW_0_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Time_Mode, param->Board->Screen);
                // Guardamos la hora configurada
                Clock_Get_Displays_Values(&actual_time, value);
                Clock_Set_Time(param->clock, &actual_time);
            } else if (events & SW_1_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Minutes_Mode, param->Board->Screen);
            } else if (events & SW_5_EVENT) {
                // Incrementa la hora
                Clock_Increment_Hours(&actual_time);
            } else if (events & SW_4_EVENT) {
                // decrementa la hora
                Clock_Decrement_Hours(&actual_time);
            }
        } else if (actual_mode == Clock_Time_Mode) {
            if ((events & SW_LONG_EVENT)) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Minutes_Mode, param->Board->Screen);
            }
            if ((events & SW_LONG_3_EVENT)) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Alarm_Mode, param->Board->Screen);
            }
            if (events & SW_0_EVENT) {
                // activa alarma
                Clock_Set_Alarm(param->clock, true);
                Select_Point_On(param->Board->Screen, 3);
            }
            if ((events & SW_1_EVENT) && !alarm_sounding) {
                // desactiva alarma
                Clock_Set_Alarm(param->clock, false);
                All_Points_Off(param->Board->Screen);
                Flash_Point(param->Board->Screen, 1, 1, 1000);
                Digital_Out_Deactivate(param->Board->Led_1);
            }
            // Si la hora actual y la hora de alarma coinciden y si no estaba sonando entonces se activa evento
            if (Clock_Alarm_Working(param->clock, &alarm_time) && !alarm_sounding) {
                Digital_Out_Activate(param->Board->Led_3);
                Digital_Out_Deactivate(param->Board->Led_1);
                alarm_sounding = true;
            }
            // Si suena la alarma y se presiona aceptar entonces se pospone 5 min
            if ((events & SW_0_EVENT) && alarm_sounding) {
                alarm_with_delay = Clock_Set_Alarm_Delay(param->clock, 5);
                Digital_Out_Activate(param->Board->Led_1);
                Digital_Out_Deactivate(param->Board->Led_3);
                alarm_sounding = false;
            }
            // Si la alarma con delay coincide con current_time entonces se apaga el indicador de snooze y suena la
            // alarma de nuevo
            if (Clock_Alarm_Working(param->clock, &alarm_with_delay) && !alarm_sounding) {
                Digital_Out_Deactivate(param->Board->Led_1);
                // Indicador de alarma activa
                Digital_Out_Activate(param->Board->Led_3);
                // bandera que indica alarma sonando
                alarm_sounding = true;
            }
            // Si suena la alarma y se presiona cancelar entonces se apaga hasta el otro dia
            if ((events & SW_1_EVENT) && alarm_sounding) {
                Digital_Out_Deactivate(param->Board->Led_3);
                Digital_Out_Deactivate(param->Board->Led_1);
                Clock_Set_Alarm(param->clock, true);
                alarm_sounding = false;
            }
        } else if (actual_mode == Clock_Set_Alarm_Mode) {
            if ((events & SW_LONG_EVENT)) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Minutes_Alarm_Mode, param->Board->Screen);
            } else if (events & SW_1_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Time_Mode, param->Board->Screen);
                All_Points_Off(param->Board->Screen);
                Flash_Point(param->Board->Screen, 1, 1, 1000);
            }
        } else if (actual_mode == Clock_Set_Minutes_Alarm_Mode) {
            if (events & SW_0_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Hours_Alarm_Mode, param->Board->Screen);
            } else if (events & SW_1_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Alarm_Mode, param->Board->Screen);
            } else if (events & SW_5_EVENT) {
                // incrementa los minutos
                Clock_Increment_Minutes(&alarm_time);
            } else if (events & SW_4_EVENT) {
                // decrementa los minutos
                Clock_Decrement_Minutes(&alarm_time);
            }
        } else if (actual_mode == Clock_Set_Hours_Alarm_Mode) {
            if (events & SW_0_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Time_Mode, param->Board->Screen);
                //
                Clock_Get_Displays_Values(&alarm_time, value);
                // actualiza el horario de la alarma
                Clock_Set_Time_Alarm(param->clock, &alarm_time);
                // actualizo el puntero al horario de la alarma
                param->alarm_time = Clock_Alarm(param->clock);
                All_Points_Off(param->Board->Screen);
                Flash_Point(param->Board->Screen, 1, 1, 1000);
                // activa alarma
                Clock_Set_Alarm(param->clock, true);
                Select_Point_On(param->Board->Screen, 3);
            } else if (events & SW_1_EVENT) {
                //  cambia el estado
                Change_Mode(param->clock, Clock_Set_Minutes_Alarm_Mode, param->Board->Screen);
            } else if (events & SW_5_EVENT) {
                // Incrementa la hora
                Clock_Increment_Hours(&alarm_time);
            } else if (events & SW_4_EVENT) {
                // decrementa la hora
                Clock_Decrement_Hours(&alarm_time);
            }
        }

        // Deteccion de Tick (1 segundo)

        if (actual_mode != Clock_Init_Mode) {
            if (events & TICK_1_SECOND_EVENT) {
                // Avanzo 1 seg
                Clock_New_Tick(param->clock);
                // Indicador de Tick cada 1 seg
                // Digital_Out_Toggle(param->Board->Led_2);
                // Actualizo el puntero a la hora actual del reloj
                param->current_time = Clock_Time(param->clock);
            }
        }

        // Escribo y actualizo displays

        if (xSemaphoreTake(param->screen_Mutex, portMAX_DELAY)) {

            if (actual_mode == Clock_Init_Mode) {
                // Escribe horario por defecto
                Screen_Write_BCD(param->Board->Screen, value, 4);
            } else if (actual_mode == Clock_Set_Hours_Mode || actual_mode == Clock_Set_Minutes_Mode) {
                // Escribe horario modificado segun hora y minutos
                Clock_Get_Displays_Values(&actual_time, value);
                Screen_Write_BCD(param->Board->Screen, value, CANT_DISPLAYS);
            } else if (actual_mode == Clock_Time_Mode) {
                // actualiza y escribe el horario actual
                actual_time = Clock_Time(param->clock);
                Clock_Get_Displays_Values(&param->current_time, value);
                Screen_Write_BCD(param->Board->Screen, value, CANT_DISPLAYS);
            } else if ((actual_mode == Clock_Set_Minutes_Alarm_Mode) || (actual_mode == Clock_Set_Hours_Alarm_Mode)) {
                // Escribe el horario de la alarma a modificar
                Clock_Get_Displays_Values(&alarm_time, value);
                Screen_Write_BCD(param->Board->Screen, value, CANT_DISPLAYS);
            } else if ((actual_mode == Clock_Set_Alarm_Mode)) {
                // Escribe el horario de la alarma
                alarm_time = Clock_Alarm(param->clock);
                Clock_Get_Displays_Values(&param->alarm_time, value);
                Screen_Write_BCD(param->Board->Screen, value, CANT_DISPLAYS);
            }
            // Libero el mutex
            xSemaphoreGive(param->screen_Mutex);
        }

        // Deteccion de inactividad

        if ((events & SW_4_EVENT) || (events & SW_LONG_3_EVENT) || (events & SW_LONG_EVENT) || (events & SW_3_EVENT) ||
            (events & SW_2_EVENT) || (events & SW_1_EVENT) || (events & SW_0_EVENT) || (events & SW_5_EVENT)) {
            // Contador de inactividad se reinicia si se detecta un evento de cualquier boton
            last_iteraction = xTaskGetTickCount();
        }
        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - last_iteraction) > pdMS_TO_TICKS(30000)) {
            if (actual_mode == Clock_Init_Mode || actual_mode == Clock_Set_Minutes_Mode ||
                actual_mode == Clock_Set_Hours_Mode) {
                // Cambio a modo de funcionamiento normal
                Change_Mode(param->clock, Clock_Time_Mode, param->Board->Screen);
                last_iteraction = current_time;
            } else if (actual_mode == Clock_Set_Minutes_Alarm_Mode || actual_mode == Clock_Set_Hours_Alarm_Mode) {
                // Cambio a modo de funcionamiento normal
                Change_Mode(param->clock, Clock_Set_Alarm_Mode, param->Board->Screen);
                last_iteraction = current_time;
            }
        }

        xEventGroupClearBits(param->clock_Events, SW_0_EVENT | SW_1_EVENT | SW_2_EVENT | SW_3_EVENT | SW_4_EVENT |
                                                      SW_5_EVENT | SW_LONG_EVENT | TICK_1_SECOND_EVENT |
                                                      SW_LONG_3_EVENT);
    }
}

/* === End of documentation** ========================================================================================*/
