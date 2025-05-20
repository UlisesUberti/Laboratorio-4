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

/** @file DigitalIn.c
 * @brief Código fuente de DigitalIn.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "DigitalIn.h"
#include "chip.h"
/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/**
 * @brief Definicion de una estructura con los parametros de una Entrada Digital
 * @param port Puerto GPIO
 * @param bit bit del puerto
 * @param inverted Logica Invertida o Directa
 * @param last_state Ultimo estado (Activo o Desactivo)
 */
struct Digital_In_s {
    uint8_t port;
    uint8_t bit;
    bool inverted;
    bool last_state;
};

/* === Private function declarations =============================================================================== */

Digital_In_t Digital_In_Create(uint8_t port, uint8_t bit, bool inverted) {

    Digital_In_t Digital_In = malloc(sizeof(struct Digital_In_s));

    if (Digital_In != NULL) {
        Digital_In->port = port;
        Digital_In->bit = bit;
        Digital_In->inverted = false;
        Digital_In->last_state = false;
        // Funcion del fabricante que la define que como entrada
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, Digital_In->port, Digital_In->bit, false);
        return Digital_In;
    }

    return Digital_In;
}

bool Digital_In_GetState(Digital_In_t Digital_In) {

    // Leo la entrada con la funcion del fabricante
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, Digital_In->port, Digital_In->bit);

    // Si tiene logica invertida entonces inverted es TRUE e invierte el estado
    if (Digital_In->inverted) {
        state = !state;
    }

    return state;
}

Digital_States_t Digital_In_Was_Changed(Digital_In_t Digital_In) {

    // Defino un tipo de dato enum con el estado de "No cambio"
    Digital_States_t result = Input_NOT_CHANGE;

    // Defino un tipo de dato bool que me diga si esta en alto (TRUE) o bajo(False)
    bool state = Digital_In_GetState(Digital_In);

    // Si estado esta en 1 y last_state en 0 --> Cambio a alto
    if (state && !Digital_In->last_state) {
        result = Input_Was_Activeted;
    }
    // Si el estado estaba en bajo y la salida en Alto -->
    else if (!state && Digital_In->last_state) {
        result = Input_Was_Deactiveted;
    }
    // Actualizo el ultimo estado
    Digital_In->last_state = state;

    return result;
}

bool Digital_In_Was_Activated(Digital_In_t Digital_In) {
    return Input_Was_Activeted == Digital_In_Was_Changed(Digital_In);
}

bool Digital_In_Was_Deactivated(Digital_In_t Digital_In) {
    return Input_Was_Deactiveted == Digital_In_Was_Changed(Digital_In);
}

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

/* === End of documentation ======================================================================================== */
