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

/** @file screen.c
 * @brief Código fuente de screen.c
 * @author Uberti, Ulises Leandro
 * */

/* === Headers files inclusions ==================================================================================== */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "screen.h"
#include "poncho.h"
/* === Macros definitions ========================================================================================== */
#ifndef SCREEN_MAX_Digits
#define SCREEN_MAX_Digits 8
#endif

#ifndef POINT
#define POINT 1
#endif
/* === Private data type declarations ============================================================================== */

/**
 * @brief Arreglo para conversion BCD a 7 segmentos
 *
 */
static const uint8_t Digit_Map[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_G | SEGMENT_E | SEGMENT_D,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_G | SEGMENT_F,                                     // 4
    SEGMENT_A | SEGMENT_F | SEGMENT_G | SEGMENT_C | SEGMENT_D,                         // 5
    SEGMENT_A | SEGMENT_F | SEGMENT_G | SEGMENT_E | SEGMENT_C | SEGMENT_D,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};

struct screen_s {
    uint8_t Digits;                   // cantidad de digitos de la pantalla (nuestro caso 4)
    uint8_t Value[SCREEN_MAX_Digits]; // Value es un arreglo que almacena que numero debe estar en cada digito
    uint8_t Current_Digit;            // el digito actual activado durante el multiplexado
    screen_driver_t Driver;           // estructura de punteros a funciones (apagar,prender,actualizar segmentos)
    uint8_t Flashing_From;            // Digito desde el que parpadea
    uint8_t Flashing_To;              // Digito hasta el que parpadea
    uint8_t Flashing_Frecuency;       // Frecuencia de parpadeo
    uint8_t Flashing_Count;           // Contador de parpadeo de displays
    uint8_t Point;                    // punto de un display
    bool Point_On;                    // Estado del punto asignado (false-apagado)
    uint8_t Flash_Point;              // Punto a parpadear de un display
    uint8_t Frecuency_Flashing_Point; // Frecuencia de parpadeo de un punto
    uint8_t Flashing_Point_Count;     // Contador de parpadeo de un punto
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t Screen_Create(screen_driver_t Driver, uint8_t Digits) {

    screen_t screen = malloc(sizeof(struct screen_s));

    if (Digits > SCREEN_MAX_Digits) {
        // Trunco en caso de que supere la cantidad maxima de digitos asignados
        Digits = SCREEN_MAX_Digits;
    }

    if (screen != NULL) {
        screen->Digits = Digits;              // Defino la cantidad de digitos
        screen->Driver = Driver;              // Defino el driver de la pantalla (funciones de callback)
        screen->Current_Digit = 0;            // Inicializo el digito actual en 0
        screen->Flashing_Frecuency = 0;       // Incializo en 0 la frecuencia de parpadeo de display
        screen->Flashing_Count = 0;           // Incilaizo en 0 el contador del parpadeo de display
        screen->Point = POINT;                // Incializo uno de los puntos
        screen->Point_On = false;             // Incializo el punto apagado
        screen->Frecuency_Flashing_Point = 0; // Inicializo en 0 la frecuencia de parpadeo del punto
        screen->Flash_Point = Digits + 1;     // Inicializo el punto de parpadeo fuera del rango dado de digitos
        screen->Flashing_Point_Count = 0;     // Inicializo en 0 el contador de parpadeo de punto
    }
    return screen;
}

void Screen_Write_BCD(screen_t screen, uint8_t Value[], uint8_t size) {
    // menset --> funcion de la libreria stdlib permite completar el array con 0
    memset(screen->Value, 0, sizeof(screen->Value));
    // Limpia la memoria de video (Value[]) para poder escribir sin problemas
    if (size > screen->Digits) {
        size = screen->Digits;
        // le asigno el tamaño maximo como la cantidad de digitos a escribir
    }
    for (uint8_t i = 0; i < size; i++) {
        screen->Value[i] = Digit_Map[Value[i]];
        // si i = 1 --> recorre el mapa hasta la posicion 1 y lo almacena en screen
        // ahora Value[] es de la forma Value[Digit_Map[i],...,Digit_Map[size]]
    }
}

void Screen_Refresh(screen_t screen) {

    uint8_t segments;

    // Apago todos los digitos
    screen->Driver.Digit_Turn_Off();
    // Posiciona al objeto en el siguiente digito (circular)
    screen->Current_Digit = (screen->Current_Digit + 1) % screen->Digits;
    // segments guarda el numero que se debe representar en 7 segmentos
    segments = screen->Value[screen->Current_Digit];
    // Si el parpadeo esta activo entra en el condicional
    if (screen->Flashing_Frecuency != 0) {
        if (screen->Current_Digit == 0) { // Si se trata del primer digito incrementa el contador
            screen->Flashing_Count = (screen->Flashing_Count + 1) % screen->Flashing_Frecuency;
        }
        // Si el contador es menor que la mitad de la frecuencia de parpadeo -> no se activa ningun segmento
        if (screen->Flashing_Count < (screen->Flashing_Frecuency / 2)) {
            if (screen->Current_Digit >= screen->Flashing_From && screen->Current_Digit <= screen->Flashing_To) {
                segments = 0;
            }
        }
    }
    // el valor de segments representa el numero a representar en el display
    screen->Driver.Segments_Turn_Update(segments);       // actualiza los segmentos
    screen->Driver.Digit_Turn_On(screen->Current_Digit); // enciende el digito actual

    // condicional para prender un punto constantemente
    if (screen->Point == screen->Current_Digit && screen->Point != screen->Flash_Point && screen->Point_On == true) {
        screen->Driver.Point_On();
    }

    // Condiconal para hacer parpadear un punto
    if (screen->Frecuency_Flashing_Point != 0) {
        if (screen->Flash_Point == screen->Current_Digit && screen->Frecuency_Flashing_Point != 0) {
            if (screen->Flash_Point < screen->Digits) {
                // Condional para ver que no se asigno un digito mayor a la cantidad disponible
                screen->Flashing_Point_Count = (screen->Flashing_Point_Count + 1) % screen->Frecuency_Flashing_Point;
                screen->Driver.Point_On();
            }
            if (screen->Flashing_Point_Count < (screen->Frecuency_Flashing_Point / 2)) {
                screen->Driver.Point_Off();
            }
        }
    }
}

int Display_Flash_Digits(screen_t screen, uint8_t from, uint8_t to, uint16_t frecuency) {
    int result = 0;
    if (from > to || from >= SCREEN_MAX_Digits || to >= SCREEN_MAX_Digits) {
        result = -1;
    } else if (!screen) {
        result = -1;
    } else {
        screen->Flashing_Frecuency = frecuency * 2; // Multiplico por 2 para que la mitad del tiempo este en ON
        screen->Flashing_From = from;               // primer digito a parpadear
        screen->Flashing_To = to;                   // ultimo digito a parpadear
        screen->Flashing_Count = 0;                 // Contador de parpadeo inicializado en 0
        result = 0;                                 // retorna 0
    }
    return result;
}

void Select_Point_On(screen_t screen, uint8_t digit) {
    screen->Point = digit;
    screen->Point_On = true;
}

int Flash_Point(screen_t screen, uint8_t digit, uint16_t frecuency) {
    int result = 0;
    if (screen == NULL || digit > screen->Digits) {
        result = -1;
    } else {
        screen->Frecuency_Flashing_Point = frecuency;
        screen->Flash_Point = digit;
        result = 0;
    }
    return result;
}

/* === End of documentation ======================================================================================== */
