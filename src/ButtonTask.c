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

/** @file ButtonTask.c
 * @brief Código fuente de ButtonTask.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include "ButtonTask.h"
#include <stdio.h>
#include "chip.h"
#include <stdbool.h>
#include "bsp.h"

/* === Macros definitions ========================================================================================== */

// Defino una variable para evitar el rebote del boton
#define SW_SCAN_DELAY 100

// defino el tiempo de duracion de un boton que permite cambiar al modo "set time"
#define LONG_DURATION_TIME 3000 // [ms]

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void Button_Task(void * args) {
    // Defino un puntero a la estructura de la tarea con los argumentos que le pase
    Button_Task_Args_t parameters = args;
    // Defino variables para un contador
    TickType_t start = 0, duration;
    // Bandera para determinar si se presiono
    bool Button_Pressed = false;
    while (true) {
        if (Digital_In_GetState(parameters->Switch) && !Button_Pressed) {
            // Si se presiono el boton
            Button_Pressed = true;
            // Utilizo una tarea del sistema operativo que lleva la cuenta en ticks del clock
            start = xTaskGetTickCount();
        } else if (!Digital_In_GetState(parameters->Switch) && Button_Pressed) {
            // Si se dejo de presionar el boton
            Button_Pressed = false;
            duration = xTaskGetTickCount() - start;
            if (duration >= pdMS_TO_TICKS(LONG_DURATION_TIME)) {
                // Si el boton se presiono durante un tiempo mayor a Long_duration
                // Indicamos mediante un evento que fue de larga duracion
                xEventGroupSetBits(parameters->clock_events, SW_LONG_DURATION_EVENT);
                xEventGroupSetBits(parameters->clock_events, parameters->event_bit);
            } else {
                xEventGroupSetBits(parameters->clock_events, parameters->event_bit);
            }
        }
        // Utilizamos un delay (espera pasiva) para evitar el rebote del boton
        vTaskDelay(pdMS_TO_TICKS(SW_SCAN_DELAY));
    }
}
/* === End of documentation ======================================================================================== */
