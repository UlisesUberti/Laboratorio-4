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
#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
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
    uint8_t Value[SCREEN_MAX_DIGITS]; // Value es un arreglo que almacena que numero debe estar en cada digito
    uint8_t Current_Digit;            // el digito actual activado durante el multiplexado
    screen_driver_t Driver;           // estructura de punteros a funciones (apagar,prender,actualizar segmentos)
    uint8_t Flashing_From;            // Digito desde el que parpadea
    uint8_t Flashing_To;              // Digito hasta el que parpadea
    uint8_t Flashing_Frequency;       // Frecuencia de parpadeo
    uint8_t Flashing_Count;           // Contador de parpadeo de displays
    uint8_t Point;                    // punto de un display
    bool Point_On;                    // Estado del punto asignado (false-apagado)
    uint8_t Flash_Point_From;         // Punto a parpadear de un display
    uint8_t Flash_Point_To;           // Punto a parpadear de un display
    uint8_t Flashing_Point_Frequency; // Frecuencia de parpadeo de un punto
    uint8_t Flashing_Point_Count;     // Contador de parpadeo de un punto
    uint8_t Union_Count;
};

/* === Private function declarations =============================================================================== */
/**
 * @brief Funcion para cambiar el estado de los segmentos y habilitar el display
 *
 * @param screen objeto pantalla
 * @param segments valor del segmento
 * @param digit numero de display
 */
void Change_Segments(screen_t screen, uint8_t segments, uint8_t digit);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t Screen_Create(screen_driver_t Driver, uint8_t Digits) {

    screen_t screen = malloc(sizeof(struct screen_s));

    if (Digits > SCREEN_MAX_DIGITS) {
        // Trunco en caso de que supere la cantidad maxima de digitos asignados
        Digits = SCREEN_MAX_DIGITS;
    }

    if (screen != NULL) {
        screen->Digits = Digits;              // Defino la cantidad de digitos
        screen->Driver = Driver;              // Defino el driver de la pantalla (funciones de callback)
        screen->Current_Digit = 0;            // Inicializo el digito actual en 0
        screen->Flashing_Frequency = 0;       // Incializo en 0 la frecuencia de parpadeo de display
        screen->Flashing_Count = 0;           // Incilaizo en 0 el contador del parpadeo de display
        screen->Point = POINT;                // Incializo uno de los puntos
        screen->Point_On = false;             // Incializo el punto apagado
        screen->Flashing_Point_Frequency = 0; // Inicializo en 0 la frecuencia de parpadeo del punto
        screen->Flashing_Point_Count = 0;     // Inicializo en 0 el contador de parpadeo de punto
        screen->Union_Count = 0;
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
    // variable de control
    uint8_t segments;
    // Apago todos los digitos
    screen->Driver.Digit_Turn_Off();
    // Posiciona al objeto en el siguiente digito (circular)
    screen->Current_Digit = (screen->Current_Digit + 1) % screen->Digits;
    // segments guarda el numero que se debe representar en 7 segmentos
    segments = screen->Value[screen->Current_Digit];

    // Condicional para parpadear solo segmentos
    if (screen->Flashing_Frequency != 0 && screen->Flashing_Point_Frequency != screen->Flashing_Frequency) {
        // Si el digito actual es el inicial entonces:
        if (screen->Current_Digit == 0) {
            // Incrementa el contador en 1
            screen->Flashing_Count = (screen->Flashing_Count + 1) % screen->Flashing_Frequency;
            // Cada vez que pase por el digito 0 (1 de 4 en este caso) aumenta en 1 el contador hasta la frecuencia de
            // parpadeo
        }
        // Si el contador es menor que la mitad de la frecuencia de parpadeo -> no se activa ningun segmento
        if (screen->Flashing_Count < (screen->Flashing_Frequency / 2)) {
            // Si el digito actual se encuentra entre los displays con segmentos a parpadear entonces:
            if (screen->Current_Digit >= screen->Flashing_From && screen->Current_Digit <= screen->Flashing_To) {
                segments = 0;
            }
        }
        // segments toma el valor dependiedo del contador
        Change_Segments(screen, segments, screen->Current_Digit);
    }

    // Si no hay nada para parpadear se ejecuta con normalidad
    if (screen->Flashing_Frequency == 0 || screen->Flashing_Point_Frequency == 0) {
        Change_Segments(screen, segments, screen->Current_Digit);
    }

    // condicional para prender un punto constantemente
    if (screen->Point == screen->Current_Digit && screen->Point_On == true) {
        screen->Driver.Point_On();
    }

    // Condicional para hacer parpadear un conjunto de puntos
    if (screen->Flashing_Point_Frequency != 0 && screen->Flashing_Point_Frequency != screen->Flashing_Frequency) {
        if (screen->Current_Digit == 0) {
            screen->Flashing_Point_Count = (screen->Flashing_Point_Count + 1) % screen->Flashing_Point_Frequency;
        }
        if (screen->Flashing_Point_Count <= screen->Flashing_Point_Frequency / 2 &&
            screen->Current_Digit >= screen->Flash_Point_From && screen->Current_Digit <= screen->Flash_Point_To) {
            screen->Driver.Point_On();

        } else if (screen->Flashing_Point_Count >= screen->Flashing_Point_Frequency / 2 &&
                   screen->Current_Digit >= screen->Flash_Point_From &&
                   screen->Current_Digit <= screen->Flash_Point_To) {
            screen->Driver.Point_Off();
        }
        screen->Driver.Digit_Turn_On(screen->Current_Digit); // enciende el digito actual
    }

    // Condicional para parpadear puntos y displays al mismo tiempo
    if (screen->Flashing_Frequency == screen->Flashing_Point_Frequency && screen->Flashing_Point_Frequency != 0) {
        // Si el digito actual es el inicial entonces:
        if (screen->Current_Digit == 0) {
            // Cuenta con una variable en conjunto para igualar tiempos de parpadeo
            screen->Union_Count = (screen->Union_Count + 1) % screen->Flashing_Frequency;
        }
        // Si el contador esta por debajo de la mitad de la frecuencia entonces:
        if (screen->Union_Count <= screen->Flashing_Frequency / 2) {
            // Si el digito actual es uno de los que debe prender el punto entonces:
            if (screen->Current_Digit >= screen->Flash_Point_From && screen->Current_Digit <= screen->Flash_Point_To) {
                screen->Driver.Point_On(); // Prende el punto
            }
            // Si el digito actual debe prender los segmentos entonces:
            if (screen->Current_Digit >= screen->Flashing_From && screen->Current_Digit <= screen->Flashing_To) {
                Change_Segments(screen, segments, screen->Current_Digit);
            }

        } else if (screen->Union_Count >= screen->Flashing_Frequency / 2) {
            // Si el digito actual es uno de los que debe apagar el punto entonces:
            if (screen->Current_Digit >= screen->Flash_Point_From && screen->Current_Digit <= screen->Flash_Point_To) {
                screen->Driver.Point_Off(); // Apaga el punto
            }
            // Si el digito actual es uno de los que debe apagar los segmentos entonces:
            if (screen->Current_Digit >= screen->Flashing_From && screen->Current_Digit <= screen->Flashing_To) {
                segments = 0;
                Change_Segments(screen, segments, screen->Current_Digit);
            }
        }
        Change_Segments(screen, segments, screen->Current_Digit);
    }
}

int Display_Flash_Digits(screen_t screen, uint8_t from, uint8_t to, uint16_t frequency) {
    int result = 0;
    if (from > to || from >= SCREEN_MAX_DIGITS || to >= SCREEN_MAX_DIGITS) {
        result = -1;
    } else if (!screen) {
        result = -1;
    } else {
        screen->Flashing_Frequency = frequency * 2; // Multiplico por 2 para que la mitad del tiempo este en ON
        screen->Flashing_From = from;               // primer digito a parpadear
        screen->Flashing_To = to;                   // ultimo digito a parpadear
        screen->Flashing_Count = 0;                 // Contador de parpadeo inicializado en 0
        result = 0;                                 // retorna 0
    }
    return result;
}

int Select_Point_On(screen_t screen, uint8_t digit) {
    int result = -1;
    if (digit < SCREEN_MAX_DIGITS) {
        screen->Point = digit;
        screen->Point_On = true;
        result = 0;
    }
    return result;
}

int Flash_Point(screen_t screen, uint8_t from, uint8_t to, uint16_t frequency) {
    int result = 0;
    if (screen == NULL || from > screen->Digits || to > screen->Digits) {
        result = -1;
    } else {
        screen->Flashing_Point_Frequency = frequency * 2; // Asigno la frecuencia de parpadeo del punto
        screen->Flash_Point_From = from;                  // Asigno el digito correspondiente al digito a prender
        screen->Flash_Point_To = to;                      // Asigno el digito correspondiente al digito a prender
        screen->Flashing_Point_Count = 0;                 // Inicializo el contador en 0
        result = 0;
    }
    return result;
}

void Change_Segments(screen_t screen, uint8_t segments, uint8_t digit) {
    screen->Driver.Segments_Turn_Update(segments); // actualiza los segmentos
    screen->Driver.Digit_Turn_On(digit);           // enciende el digito actual
}
/* === End of documentation ========================================================================================
 */
