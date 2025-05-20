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

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "chip.h"
#include <stdbool.h>
#include "DigitalOut.h"
#include "DigitalIn.h"
// #include "EDU-CIAA.h"
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

    int divisor = 0;

    // Defino el led rojo del RGB
    // Chip_SCU_PinMuxSet(LED_R_PORT, LED_R_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_R_FUNC);
    // Digital_Out_t led_R = Digital_Out_Create(LED_R_GPIO, LED_R_BIT);

    // Defino el led verde del RGB
    // Chip_SCU_PinMuxSet(LED_G_PORT, LED_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_G_FUNC);
    // Digital_Out_t Led_G = Digital_Out_Create(LED_G_GPIO, LED_G_BIT);

    // Defino el led azul del RGB
    // Chip_SCU_PinMuxSet(LED_B_PORT, LED_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_B_FUNC);
    // Digital_Out_t led_B = Digital_Out_Create(LED_B_GPIO, LED_B_BIT);

    /******************/

    // Defino el LED 1 de la placa (Rojo)
    // Chip_SCU_PinMuxSet(LED_1_PORT, LED_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_1_FUNC);
    // Digital_Out_t Led_Red = Digital_Out_Create(LED_1_GPIO, LED_1_BIT);

    // Defino el LED 2 de la placa (Amarillo)
    // Chip_SCU_PinMuxSet(LED_2_PORT, LED_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_2_FUNC);
    // Digital_Out_t Led_Yellow = Digital_Out_Create(LED_2_GPIO, LED_2_BIT);

    // Defino el LED 3 de la placa (Verde)
    // Chip_SCU_PinMuxSet(LED_3_PORT, LED_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_3_FUNC);
    // Digital_Out_t Led_Green = Digital_Out_Create(LED_3_GPIO, LED_3_BIT);

    /******************/

    // Defino la entrada digital de la TECLA 1
    // Chip_SCU_PinMuxSet(TEC_1_PORT, TEC_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_1_FUNC);
    // Digital_In_t Sw1 = Digital_In_Create(TEC_1_GPIO, TEC_1_BIT, false);

    // Defino la entrada digital de la TECLA 2
    // Chip_SCU_PinMuxSet(TEC_2_PORT, TEC_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_2_FUNC);
    // Digital_In_t Sw2 = Digital_In_Create(TEC_2_GPIO, TEC_2_BIT, false);

    // Defino la entrada digital de la TECLA 3
    // Chip_SCU_PinMuxSet(TEC_3_PORT, TEC_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_3_FUNC);
    // Digital_In_t Sw3 = Digital_In_Create(TEC_3_GPIO, TEC_3_BIT, false);

    // Defino la entrada digital de la TECLA 4
    // Chip_SCU_PinMuxSet(TEC_4_PORT, TEC_4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_4_FUNC);
    // Digital_In_t Sw4 = Digital_In_Create(TEC_4_GPIO, TEC_4_BIT, false);

    //
    Board_t Board = Board_Create();

    // loop de programa
    while (true) {

        // Si se activa la TECLA 1 entonces se prende el LED RGB
        if (Digital_In_GetState(Board->Sw1) == 0) {

            Digital_Out_Activate(Board->Led_B);

        } else {
            // Caso contrario se desactiva
            Digital_Out_Deactivate(Board->Led_B);
        }

        // Si la TECLA 2 estaba desactivada y se activa entonces cambia el estado del LED 1
        if (Digital_In_Was_Deactivated(Board->Sw2)) {
            Digital_Out_Toggle(Board->Led_Red);
        }

        // Si la TECLA 3 se activa entonces se activa el LED 2
        if (Digital_In_GetState(Board->Sw3) == 0) {
            Digital_Out_Activate(Board->Led_Yellow);
        }
        // Si la TECLA 4 se activa entonces se desactiva el LED 2
        if (Digital_In_GetState(Board->Sw4) == 0) {
            Digital_Out_Deactivate(Board->Led_Yellow);
        }

        // El LED 3 se activa y desactiva t[s]
        divisor++;
        if (divisor == 5) {
            divisor = 0;
            Digital_Out_Toggle(Board->Led_Green);
        }

        for (int index = 0; index < 100; index++) {
            for (int delay = 0; delay < 25000; delay++) {
                __asm("NOP"); // instruccuion para que no figure como vacio
            }
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
