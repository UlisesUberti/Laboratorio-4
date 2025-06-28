/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @brief Codigo fuente del archivo main.c
 ** @author Uberti, Ulises Leandro
 ** @file main.c
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include "DigitalOut.h"
#include "DigitalIn.h"
#include "bsp.h"
#include "clock.h"

/* === Macros definitions ====================================================================== */
#define CANT_DISPLAYS 4
/* === Private data type declarations ========================================================== */
typedef enum {
    Clock_Init_Mode,
    Clock_Time_Mode,
    Clock_Set_Minutes_Mode,
    Clock_Set_Hours_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Change_Time_Mode,
    Clock_Set_Minutes_Alarm_Mode,
    Clock_Set_Hours_Alarm_Mode,
} Clock_Mode_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion para obtener los valores de los segmentos de los displays
 *
 * @param clock_time puntero al objeto reloj
 * @param value arreglo con los segmentos en BCD
 */
static void Clock_Get_Displays_Values(clock_time_t * clock_time, uint8_t value[]);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void Clock_Get_Displays_Values(clock_time_t * clock_time, uint8_t value[]) {
    value[0] = clock_time->time.hours[1];
    value[1] = clock_time->time.hours[0];
    value[2] = clock_time->time.minutes[1];
    value[3] = clock_time->time.minutes[0];
}

/* === Public function implementation ========================================================= */

int main(void) {

    // Estructura con los punteros a las entradas y salidas digitales de la EDU-CIAA
    Board_t Board = Board_Create();
    // Creo el objeto Reloj
    clock_t Clock = Clock_Create(100);
    // Incializo Tick
    Init_Tick();
    // Inicializo el reloj en 00:00:00
    clock_time_t init_time = {0};
    Clock_Get_Time(Clock, &init_time);
    // Verifico que correctamente se inicializo en 00:00:00
    Clock_Set_Time(Clock, &init_time);
    // Escribo la pantalla
    clock_time_t clock_Time = Clock_Time(Clock);
    uint8_t value[4] = {0};
    uint8_t alarm_value[4] = {0};
    // Screen_Write_BCD(Board->Screen, value, 4);
    // Defino un estado incial del reloj
    Clock_Mode_t actual_mode = Clock_Init_Mode;
    // Defino un horario de alarma por defecto
    clock_time_t alarm_time = {0};
    // Defino un puntero que guarde la direccion de la hora mientras esta en otro proceso
    clock_time_t clock_actual_time;
    // Variable para llevar la cuenta inicial de 1 segundo
    uint32_t last_time = 0;
    uint32_t refresh = 0;

    // variable para incializar una sola vez el parpadeo
    bool init;

    // Variable que avisa si es la primera inicializacion de tiempo
    bool first_set = true;

    // variable para indicar que la alarma cambio su horario por defecto
    bool first_set_alarm = true;

    // variable para evitar el rebota
    while (true) {
        switch (actual_mode) {
        case Clock_Init_Mode:
            if (!init) {
                // Todos los displays se inicializan parpadeando
                Display_Flash_Digits(Board->Screen, 0, 3, 250);
                // Se inicializa parpadeando el segundo punto
                Flash_Point(Board->Screen, 1, 1, 250);
                // incializo el parpadeo
                init = true;
            }

            // Escribo la pantalla
            Screen_Write_BCD(Board->Screen, value, 4);

            if (Board_getMillis() - refresh >= 1) {
                // Muestro por pantalla
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }

            if (Digital_In_Was_Activated(Board->Set_Time)) {
                actual_mode = Clock_Set_Minutes_Mode;
                init = false;
                first_set = false;
                refresh = 0;
            }

            break;

        case Clock_Set_Minutes_Mode:
            // hago parpadear los minutos
            if (!init) {
                Display_Flash_Digits(Board->Screen, 2, 3, 150);
                init = true;
            }

            // Si se presiona para incrmentar el tiempo
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Minutes(&clock_Time);
            }

            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Minutes(&clock_Time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);

            // Refresco la pantalla cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Activated(Board->Accept)) {
                    __asm("NOP");
                }
                actual_mode = Clock_Set_Hours_Mode;
                init = false;
                refresh = 0;
            }

            // Si se presiona cancelar pasamos al modo inicial o al modo normal dependiendo de donde estabamos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Changed(Board->Cancel)) {
                    __asm("NOP");
                }
                if (first_set) {
                    actual_mode = Clock_Init_Mode;
                    first_set = false;

                } else {
                    actual_mode = Clock_Time_Mode;
                }
                refresh = 0;
                init = false;
            }

            break;

        case Clock_Set_Hours_Mode:
            // hago parpadear la hora
            if (!init) {
                Display_Flash_Digits(Board->Screen, 0, 1, 150);
                init = true;
            }

            // Si se presiona para incrementar la hora
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Hours(&clock_Time);
                while (Digital_In_Was_Changed(Board->Increment)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }
            // Si se presiona para decrementar la hora
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Hours(&clock_Time);
                while (Digital_In_Was_Changed(Board->Decrement)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);

            // Contador para pasar un segundo por interrupcion
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Changed(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                refresh = 0;
                init = false;
                actual_mode = Clock_Time_Mode;
                Clock_Set_Time(Clock, &clock_Time);
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Deactivated(Board->Cancel)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Minutes_Mode;
                refresh = 0;
                init = false;
            }
            break;

        case Clock_Time_Mode:
            if (!init) {
                init = true;
                Flash_Point(Board->Screen, 1, 1, 500);
                Display_Flash_Digits(Board->Screen, 0, 3, 0);
            }

            if (Board_getMillis() - last_time >= 1000) {
                // Actualizo last_time
                last_time = Board_getMillis();
                // Condicion para determinar si avanzo 1 seg
                Clock_New_Tick(Clock);
                // Actualizo la hora del reloj
                Clock_Get_Time(Clock, &clock_Time);
                // Actualizo el valor de los displays
                Clock_Get_Displays_Values(&clock_Time, value);
                Screen_Write_BCD(Board->Screen, value, 4);
            }

            // Refresco cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }

            // Si se presiona Set_Time pasamos a mdoificar los minutos
            if (Digital_In_Was_Activated(Board->Set_Time)) {
                while (Digital_In_Was_Deactivated(Board->Set_Time)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                init = false;
                refresh = 0;
                actual_mode = Clock_Set_Minutes_Mode;
                last_time = 0;
            }
            if (Clock_Alarm_Working(Clock, &alarm_time)) {
                Digital_Out_Activate(Board->Buzzer);
            }

            // Si se presiona el boton de alarma pasa al estado set_alarma
            if (Digital_In_Was_Activated(Board->Set_Alarm)) {
                while (Digital_In_Was_Activated(Board->Set_Alarm)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Alarm_Mode;
                init = false;
                refresh = 0;
                last_time = 0;
            }
            if (Digital_In_Was_Activated(Board->Accept)) {
                Clock_Set_Alarm(Clock, true);
            }

            break;

        case Clock_Set_Alarm_Mode:
            if (!init) {
                init = true;
                Display_Flash_Digits(Board->Screen, 0, 3, 0);
                All_Points_On(Board->Screen);
            }
            if (first_set_alarm) {
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                first_set_alarm = false;
            }

            // Muestro por pantalla el horario de la alarma
            Clock_Get_Displays_Values(&alarm_time, value);
            Screen_Write_BCD(Board->Screen, value, CANT_DISPLAYS);

            // Refresco cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }

            // Si se presiona Set_Time se setea el horario de la alarma
            if (Digital_In_Was_Activated(Board->Set_Time)) {
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
                init = false;
                refresh = 0;
            }

            // Si se presiona el boton de alarma vuelve nuevamente a clock_time_mode
            if (Digital_In_Was_Activated(Board->Set_Alarm)) {
                actual_mode = Clock_Time_Mode;
                init = false;
                refresh = 0;
                All_Points_Off(Board->Screen);
            }
            break;
        case Clock_Set_Minutes_Alarm_Mode:
            // hago parpadear los minutos
            if (!init) {
                Display_Flash_Digits(Board->Screen, 2, 3, 200);
                init = true;
            }

            // Si se presiona para incrmentar el tiempo
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Minutes(&alarm_time);
            }

            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Minutes(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Refresco la pantalla cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Activated(Board->Accept)) {
                    __asm("NOP");
                }
                refresh = 0;
                actual_mode = Clock_Set_Hours_Alarm_Mode;
                init = false;
            }

            // Si se presiona cancelar pasamos al modo inicial o al modo normal dependiendo de donde estabamos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Changed(Board->Cancel)) {
                    __asm("NOP");
                }
                actual_mode = Clock_Set_Alarm_Mode;
                refresh = 0;
                init = false;
            }
            break;

        case Clock_Set_Hours_Alarm_Mode:
            // hago parpadear la hora
            if (!init) {
                Display_Flash_Digits(Board->Screen, 0, 1, 200);
                init = true;
            }

            // Si se presiona para incrementar la hora
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Hours(&alarm_time);
            }
            // Si se presiona para decrementar la hora
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Hours(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Contador para pasar un segundo por interrupcion
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Changed(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                init = false;
                actual_mode = Clock_Time_Mode;
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                refresh = 0;
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Deactivated(Board->Cancel)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
                refresh = 0;
                init = false;
            }
            break;

        default:
            break;
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
