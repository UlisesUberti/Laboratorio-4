
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

#ifndef SCREEN_H_
#define SCREEN_H_

/** @file screen.h
 ** @brief Codigo fuente de screen.h
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include "DigitalIn.h"
#include "DigitalOut.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estructura del objeto pantalla
 *
 */
typedef struct screen_s * screen_t;

// funciones callback sintaxis --> typedef Dato_Retornar (*puntero_funcion)(argumento);

typedef void (*Digits_Turn_Off_t)(void); // apaga los segmentos

typedef void (*Digit_Turn_On_t)(uint8_t); // enciende el digito que corresponda

typedef void (*Segments_Turn_Update_t)(uint8_t); // enciende los segmentos correspondientes

typedef void (*Point_Off_t)(void); // Apaga el punto

typedef void (*Point_On_t)(void); // Enciende el punto

// Estructura con los punteros a las funciones callback de la pantalla
typedef struct screen_driver_s {
    Digit_Turn_On_t Digit_Turn_On;               // Funcion para encender un display
    Digits_Turn_Off_t Digit_Turn_Off;            // Funcion para apagar los displays
    Segments_Turn_Update_t Segments_Turn_Update; // Funcion para encender los segmentos
    Point_Off_t Point_Off;
    Point_On_t Point_On;
} screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear el objeto pantalla
 *
 * @param Driver Funciones a realizar por la pantalla
 * @param Num_Digits Cantidad de displays
 * @return screen_t puntero a la pantalla
 */
screen_t Screen_Create(screen_driver_t Driver, uint8_t Num_Digits);

/**
 * @brief Funcion para escribir en pantalla
 *
 * @param Screen Pantalla a escribir
 * @param Value Digitos a escribir
 * @param Size Tamaño de la pantalla
 */
void Screen_Write_BCD(screen_t Screen, uint8_t Value[], uint8_t Size);

/**
 * @brief Funcion para multiplexar la pantalla
 *
 * @param screen Pantalla
 */
void Screen_Refresh(screen_t screen);

/**
 * @brief Funcion para hacer parpeadear un display 7 segmentos
 *
 * @param screen puntero al objeto pantalla
 * @param from Primer digito que parpadeara
 * @param to Ultimo digito que parpadeara
 * @param frecuency frecuencia de parpadeo
 */
int Display_Flash_Digits(screen_t screen, uint8_t from, uint8_t to, uint16_t frecuency);

/**
 * @brief Funcion para seleccionar un punto de los displays a prender
 *
 * @param screen Pantalla creada
 * @param digit numero de display
 */
void Select_Point_On(screen_t screen, uint8_t digit);

/**
 * @brief Funcion para asignar parametros de parpadeo a un punto
 *
 * @param screen Pnatalla creada
 * @param digit Numero de display
 * @param frecuency Frecuencia de parpadeo
 * @return int (0) si se asigna sin incovenientes los parametros
 */
int Flash_Point(screen_t screen, uint8_t digit, uint16_t frecuency);

/* === Public function declarations ================================================================================ */

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
