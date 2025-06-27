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

/* === Private data type declarations ========================================================== */
typedef enum {
    Clock_Init_Mode,
    Clock_Time_Mode,
    Clock_Set_Minutes_Mode,
    Clock_Set_Hours_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Alarm_Working_Mode,
    Clock_Change_Time_Mode,
} Clock_Mode_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

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
    // Screen_Write_BCD(Board->Screen, value, 4);
    // Defino un estado incial del reloj
    Clock_Mode_t actual_mode = Clock_Init_Mode;

    // Variable para llevar la cuenta inicial de 1 segundo
    uint32_t last_time = 0;
    uint32_t last_time_minutes = 0;
    uint32_t last_time_hours = 0;
    uint32_t last_refresh = 0;
    uint32_t last_time_mode = 0;

    // variable para incializar una sola vez el parpadeo
    bool init_mode_flash = false, init_flash_minutes = false, init_flash_hours = false;
    bool init_time_mode = false;
    // Variable que avisa si es la primera inicializacion de tiempo
    bool first_set = true;
    // variable para evitar el rebota
    while (true) {
        switch (actual_mode) {
        case Clock_Init_Mode:
            if (!init_mode_flash) {
                // Todos los displays se inicializan parpadeando
                Display_Flash_Digits(Board->Screen, 0, 3, 250);
                // Se inicializa parpadeando el segundo punto
                Flash_Point(Board->Screen, 1, 1, 250);
                Screen_Write_BCD(Board->Screen, value, 4);
                // incializo el parpadeo
                init_mode_flash = true;
            }

            if (Board_getMillis() - last_refresh >= 1) {
                // Muestro por pantalla
                Screen_Refresh(Board->Screen);
                last_refresh = Board_getMillis();
            }

            // for (int index = 0; index < 25000; index++) {
            //   __asm("NOP"); // instruccion para que no figure como vacio
            //}

            if (Digital_In_Was_Activated(Board->Set_Time)) {
                actual_mode = Clock_Set_Minutes_Mode;
                init_mode_flash = false;
                first_set = false;
            }

            break;

        case Clock_Set_Minutes_Mode:
            // prendo el punto del segundo display
            // Select_Point_On(Board->Screen, 1);
            // hago parpadear los minutos
            if (!init_flash_minutes) {
                Display_Flash_Digits(Board->Screen, 2, 3, 250);
                init_flash_minutes = true;
            }

            // Si se presiona para incrmentar el tiempo
            if (Digital_In_Was_Activated(Board->Increment)) {
                clock_Time.time.minutes[0]++;
                if (clock_Time.time.minutes[0] == 10) {
                    clock_Time.time.minutes[0] = 0;
                    clock_Time.time.minutes[1]++;
                }
                if (clock_Time.time.minutes[1] == 6) {
                    clock_Time.time.minutes[1] = 0;
                }
            }
            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                if (clock_Time.time.minutes[0] == 0 && clock_Time.time.minutes[1] == 0) {
                    clock_Time.time.minutes[1] = 5;
                    clock_Time.time.minutes[0] = 9;
                } else if (clock_Time.time.minutes[0] == 0) {
                    clock_Time.time.minutes[0] = 9;
                    clock_Time.time.minutes[1]--;
                } else {
                    clock_Time.time.minutes[0]--;
                }
            }
            // Actualizo el valor de los displays
            value[0] = clock_Time.time.hours[1];
            value[1] = clock_Time.time.hours[0];
            value[2] = clock_Time.time.minutes[1];
            value[3] = clock_Time.time.minutes[0];
            Screen_Write_BCD(Board->Screen, value, 4);

            if (Board_getMillis() - last_time_minutes >= 1) {
                last_time_minutes = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Changed(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Hours_Mode;
                init_flash_minutes = false;
            }
            // Si se presiona cancelar pasamos al modo inicial
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

                init_flash_minutes = false;
            }

            break;

        case Clock_Set_Hours_Mode:
            // hago parpadear la hora
            if (!init_flash_hours) {
                Display_Flash_Digits(Board->Screen, 0, 1, 250);
                init_flash_hours = true;
            }

            if (Digital_In_Was_Activated(Board->Increment)) {
                clock_Time.time.hours[0]++;
                if (clock_Time.time.hours[0] == 10) {
                    clock_Time.time.hours[0] = 0;
                    clock_Time.time.hours[1]++;
                }
                if ((clock_Time.time.hours[1] == 2 && clock_Time.time.hours[0] == 4) ||
                    (clock_Time.time.hours[1] == 3)) {
                    clock_Time.time.hours[1] = 0;
                    clock_Time.time.hours[0] = 0;
                }
                while (Digital_In_Was_Changed(Board->Increment)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }
            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                if (clock_Time.time.hours[0] == 0 && clock_Time.time.hours[1] == 0) {
                    clock_Time.time.hours[1] = 2;
                    clock_Time.time.hours[0] = 3;
                } else if (clock_Time.time.hours[0] == 0) {
                    clock_Time.time.hours[1]--;
                    if (clock_Time.time.hours[1] == 2) {
                        clock_Time.time.hours[0] = 3;
                    } else {
                        clock_Time.time.hours[0] = 9;
                    }
                } else {
                    clock_Time.time.hours[0]--;
                }
                while (Digital_In_Was_Changed(Board->Decrement)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }

            // Actualizo el valor de los displays
            value[0] = clock_Time.time.hours[1];
            value[1] = clock_Time.time.hours[0];
            value[2] = clock_Time.time.minutes[1];
            value[3] = clock_Time.time.minutes[0];
            Screen_Write_BCD(Board->Screen, value, 4);

            // Contador para pasar un segundo por interrupcion
            if (Board_getMillis() - last_time_hours >= 1) {
                last_time_hours = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Deactivated(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                init_flash_hours = false;
                actual_mode = Clock_Time_Mode;
                Clock_Set_Time(Clock, &clock_Time);
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Deactivated(Board->Cancel)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Minutes_Mode;
            }
            break;

        case Clock_Time_Mode:
            if (!init_time_mode) {
                init_time_mode = true;
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
                value[0] = clock_Time.time.hours[1];
                value[1] = clock_Time.time.hours[0];
                value[2] = clock_Time.time.minutes[1];
                value[3] = clock_Time.time.minutes[0];
                Screen_Write_BCD(Board->Screen, value, 4);
            }
            if (Board_getMillis() - last_time_mode >= 1) {
                Screen_Refresh(Board->Screen);
                last_time_mode = Board_getMillis();
            }

            // Si se presiona cancelar pasamos al modo inicial
            if (Digital_In_Was_Activated(Board->Set_Time)) {
                while (Digital_In_Was_Deactivated(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                init_time_mode = false;
                actual_mode = Clock_Set_Minutes_Mode;
            }
            break;

        default:
            break;
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
