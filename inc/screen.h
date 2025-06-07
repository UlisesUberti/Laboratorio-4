
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
// defines para formalizar que el segmentoA es el bit 1 ... el segmento F el bit 6
#define SEGMENT_A (1 << 0) // 1 desplazado 0 bits a la izq
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)
/* === Public data type declarations =============================================================================== */

// Esta archivo representa a la pantalla como clase
// la idea es que pueda expendarse a una pantalla con N displays de 7 segmentos + ´punto
// como funcion debe tener una que pueda "mostrar"
// como manejo el multiplexado?? prender de a un display en tiempos inperceptibles para el ojo
// manejo el enable de cada display
// puedo usar una funcion tick()
// la pantalla debe tener una memoria de lo que va a mostrar

// creo la etructura correspondiente al objeto pantalla y un puntero a la misma
typedef struct screen_s * screen_t; //{
//  Digit_Turn_On_t DigitTurnOff;
//};

// funciones callback sintaxis --> typedef Dato_Retornar (*puntero_funcion)(argumento);

typedef void (*Digits_Turn_Off_t)(void);         // apaga los segmentos
typedef void (*Digit_Turn_On_t)(uint8_t);        // enciende el digito que corresponda
typedef void (*Segments_Turn_Update_t)(uint8_t); // enciende los segmentos correspondientes
typedef void (*Point_Off_t)(void);
typedef void (*Point_On_t)(void);
//  Segments debe recibir un componente de la lista del mapeo

// estructura del driver a pantalla que permite la abstraccion de hardware requerida (HAL)
typedef struct screen_driver_s {
    /* data */
    Digit_Turn_On_t Digit_Turn_On;
    Digits_Turn_Off_t Digit_Turn_Off;
    Segments_Turn_Update_t Segments_Turn_Update;
    Point_Off_t Point_Off;
    Point_On_t Point_On;
} screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

// La funcione que crea la pantalla, con los parametros que necesita
screen_t Screen_Create(screen_driver_t driver, uint8_t Num_Digits); // devuelve un puntero a screen_s

// funcion que no retnorna nada pero escribe en pantalla, recibe la pantalla
void Screen_Write_BCD(screen_t Screen, uint8_t Value[], uint8_t Size);

// Una funcion de refresco para el multiplexado
void Screen_Refresh(screen_t screen);

// funcion para el parpadeo
/**
 * @brief Funcion para hacer parpeadear un display 7 segmentos
 *
 * @param screen puntero al objeto pantalla
 * @param from Primer digito que parpadeara
 * @param to Ultimo digito que parpadeara
 * @param frecuency frecuencia de parpadeo
 */
int Display_Flash_Digits(screen_t screen, uint8_t from, uint8_t to, uint16_t frecuency);

// Seleccionar punto para encendido constante
void Select_Point(screen_t screen, uint8_t digit);

// Funcion para parpadear el punto
int Flash_Point(screen_t screen, uint8_t digit, uint16_t frecuency);

// void Point_Turn_On(screen_t screen, uint8_t digit);
// void Point_Turn_Off(screen_t screen, uint8_t digit);
//  Funcion para parpadear un display
//  int Point_Flash(screen_t screen, uint8_t display, uint16_t frecuency);
/* === Public function declarations ================================================================================ */

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
