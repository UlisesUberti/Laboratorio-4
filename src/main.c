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

/** @brief Codigo fuente del archivo main
 ** @author Uberti, Ulises Leandro
 ** @file main.c
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include "DigitalOut.h"
#include "DigitalIn.h"
#include "bsp.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */

int main(void) {

    int tens_hour = 0, units_hour = 0, tens_minute = 0, units_minute = 0;
    uint8_t value[4] = {tens_hour, units_hour, tens_minute, units_minute};

    // Estructura con los punteros a las entradas y salidas digitales de la EDU-CIAA
    Board_t Board = Board_Create();

    int flash = 0, flash_P = 0;
    // Escribo la pantalla
    Screen_Write_BCD(Board->Screen, value, 4);
    // Asigno un punto parpadeante
    flash_P = Flash_Point(Board->Screen, 1, 1, 100);
    // Asigno displays parpadeantes
    // flash = Display_Flash_Digits(Board->Screen, 0, 3, 100);
    Select_Point_On(Board->Screen, 3);

    while (true) {
        uint8_t value_2[4] = {tens_hour, units_hour, tens_minute, units_minute};
        // Rescribo la pantalla con valor actualizado
        Screen_Write_BCD(Board->Screen, value_2, 4);

        if (Digital_In_Was_Changed(Board->Increment)) {
            units_hour++;
            if (units_hour == 10) {
                units_hour = 0;
            }
        }
        // Refresco la pantalla
        Screen_Refresh(Board->Screen);

        for (int index = 0; index < 25000; index++) {
            __asm("NOP"); // instruccuion para que no figure como vacio
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
