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
struct Digital_In_s {
    uint8_t port;
    uint8_t bit;
    bool activo;
};

/* === Private function declarations =============================================================================== */
Digital_In_t Digital_In_Create(uint8_t port, uint8_t bit) {
    Digital_In_t Digital_In = malloc(sizeof(struct Digital_In_s));
    if (Digital_In != NULL) {
        Digital_In->port = port;
        Digital_In->bit = bit;
    }
}

bool Digital_In_GetState(Digital_In_t Digital_In) {
    return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, Digital_In->port, Digital_In->bit);
}

bool Digital_In_Was_Activated(Digital_In_t) {
}

bool Digital_In_Was_Deactivated(Digital_In_t) {
}

bool Digital_In_Was_Changed(Digital_In_t) {
}

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

/* === End of documentation ======================================================================================== */
