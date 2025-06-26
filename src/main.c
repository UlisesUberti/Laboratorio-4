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
    Clock_Time_Mode,
    Clock_Set_Time_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Alarm_Working_Mode,
    Clock_Invalid_Time_Mode,
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
    // Inicializo el reloj en 00:00:00
    clock_time_t init_time = {0};
    Clock_Get_Time(Clock, &init_time);
    // Verifico que correctamente se inicializo en 00:00:00
    Clock_Set_Time(Clock, &init_time);
    // Escribo la pantalla
    clock_time_t clock_Time = Clock_Time(Clock);
    uint8_t value[4] = {clock_Time.time.hours[0], clock_Time.time.hours[1], clock_Time.time.minutes[0],
                        clock_Time.time.minutes[1]};
    Screen_Write_BCD(Board->Screen, value, 4);
    // Defino un estado incial del reloj
    Clock_Mode_t actual_mode = Clock_Time_Mode;

    while (true) {
        switch (actual_mode) {
        case Clock_Time_Mode:
            // En este estado el reloj funciona normal mostrando la hora y prendiendo el segundo punto
            Select_Point_On(Board->Screen, 1);
            // Refresco la pantalla
            Screen_Refresh(Board->Screen);

            break;

        default:
            break;
        }
        // Refresco la pantalla
        Screen_Refresh(Board->Screen);

        for (int index = 0; index < 25000; index++) {
            __asm("NOP"); // instruccion para que no figure como vacio
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
