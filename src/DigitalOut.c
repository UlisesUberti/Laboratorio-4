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

/** @file DigitalOut.c
 * @brief Código fuente de DigitalOut
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include <stdio.h>
#include "chip.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "DigitalOut.h"
/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */
struct Digital_Out_s {
    uint8_t port;
    uint8_t pin;
    bool activo;
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

Digital_Out_t Digital_Out_Create(uint8_t port, uint8_t pin) {

    Digital_Out_t Digital_out = malloc(sizeof(struct Digital_Out_s));

    if (Digital_out != NULL) {
        Digital_out->pin = pin;
        Digital_out->port = port;
        // Funciones del fabricante para definirla como salida y para asignarle un estado inicial (false==0)
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, Digital_out->port, Digital_out->pin, true);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, Digital_out->port, Digital_out->pin, false);
    }

    return Digital_out;
}

void Digital_Out_Activate(Digital_Out_t Digital_Out) {
    // Funcion del fabricante que activa la salida
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, Digital_Out->port, Digital_Out->pin, true);
}

void Digital_Out_Deactivate(Digital_Out_t Digital_Out) {
    // Funcion del fabricante para desactivar una salida
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, Digital_Out->port, Digital_Out->pin, false);
}

void Digital_Out_Toggle(Digital_Out_t Digital_Out) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, Digital_Out->port, Digital_Out->pin);
}

/* === Public function implementation ============================================================================== */

/* === End of documentation ======================================================================================== */
