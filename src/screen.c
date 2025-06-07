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

// LO MISMO CON EL RESTO DE SEGMENTOS --> ASIGNANDOLE A CADA SEGNMENTO EL BIT CORRESPONDIENTE AL HARDWARE

/* === Private data type declarations ============================================================================== */

// defino el mapeo de los segmentos segun el numero
// Digit_Map es una lista con 9 componentes cada uno correspondiente a los segmentos que deben prenderse segun el numero
// a mostrar
// | es el operador OR binario
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
// defino la estrucutura del objeto pantalla
struct screen_s {
    uint8_t Digits;                   // cantidad de digitos de la pantalla (nuestro caso 4)
    uint8_t Value[SCREEN_MAX_Digits]; // Value es un arreglo que almacena que numero debe estar en cada digito
    uint8_t Current_Digit;            // el digito actual activado durante el multiplexado
    screen_driver_t Driver;           // estructura de punteros a funciones (apagar,prender,actualizar segmentos)
    // esta estrucutra permite que screen no depende directamente del hardware
    uint8_t Flashing_From;
    uint8_t Flashing_To;
    uint8_t Flashing_Frecuency;
    uint8_t Flashing_Count;
    uint8_t Point; // punto de un display
    uint8_t Flash_Point;
    uint8_t Frecuency_Flashing_Point;
    uint8_t Flashing_Point_Count;
};

/* === Private function declarations =============================================================================== */

// Funcion Definicion de los digitos
// Funcion que permite definir los pines GPIO del enable de cada display
void Digits_Init(void);

// funcion para la definicion de los segmentos
// Funcion que define los segmentos de un display del A al G
void Segments_Init(void);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t Screen_Create(screen_driver_t Driver, uint8_t Digits) {
    // el argumento Driver debe definirse en el programa principal
    // asigno espacio en memoria para la estrucutura del objeto pantalla
    screen_t screen = malloc(sizeof(struct screen_s));
    if (Digits > SCREEN_MAX_Digits) {
        // caso en el que se intente poner mas digitos que el ancho de la pantalla
        Digits = SCREEN_MAX_Digits;
    }
    if (screen != NULL) {
        // me aseguro que le asigne una direc al puntero
        screen->Digits = Digits;
        // le asigno al campo de digitos de la pantalla la cantidad de digitos
        // agrego las funciones del fabricante que definen a los digitos --> SCU-pinmuxset
        //  CHIP_GPIO_ Setpinstate ... en false para apagar los digitos
        //  se agrega la funcion clear Value tmb
        screen->Driver = Driver;
        screen->Current_Digit = 0;
        // con als funciones internas llamo directamente a Digits init y Digits segment
        // le asigno valores iniciales nulos a los campos del parpadeo
        screen->Flashing_Frecuency = 0;
        // incializo el contador de paparpadeo en 0
        screen->Flashing_Count = 0;
        // Point deberia recibir el display en el que debe activarse, al crearlo deberia ser el segundo punto
        screen->Point = 1;
        // Le asigno una frecuencia de parpadeo de 0 al crearlo
        screen->Frecuency_Flashing_Point = 0;
        // Le asigno un valor al punto parpadeante mayor al a la cantidad de digitos
        screen->Flash_Point = Digits + 1;
        // Contador para el parpadeo del punto se incializa en 0
        screen->Flashing_Point_Count = 0;
    }
    return screen;
}

// Funcion que convierte de BCD a 7 segmentos y guarda en memoria de video
void Screen_Write_BCD(screen_t screen, uint8_t Value[], uint8_t size) {
    // menset --> funcion de la libreria stdlib permite completar el array con 0
    memset(screen->Value, 0, sizeof(screen->Value));
    // Limpia la memoria de video (Value[]) para poder escribir sin problemas
    //  Value trae numeros del 0 al 9
    //  la memoria de video debera guardar los segmentos a escribir
    //  CONDICIONAL POR EL TAMAÑO
    //  size es la cantidad de digitos a mostrar
    //  Value [] contiene el arreglo de numeros a escribir
    if (size > screen->Digits) {
        size = screen->Digits; // le asigno el tamaño maximo como la cantidad de digitos a escribir
    }
    for (uint8_t i = 0; i < size; i++) {
        screen->Value[i] = Digit_Map[Value[i]];
        // si i = 1 --> recorre el mapa hasta la posicion 1 y lo almacena en screen
        // ahora Value[] es de la forma Value[Digit_Map[i],...,Digit_Map[size]]
    }
}

// Funcion para actualizar el estado del display
void Screen_Refresh(screen_t screen) {

    // ApagarDigitos(); // enable del display (los digitos son los enables del display)

    uint8_t segments;
    uint8_t point;
    // Apago todos los digitos
    screen->Driver.Digit_Turn_Off();
    // Posiciona al objeto en el siguiente digito (circular)
    screen->Current_Digit = (screen->Current_Digit + 1) % screen->Digits;
    // segments guarda el numero que se debe representar en 7 segmentos
    segments = screen->Value[screen->Current_Digit];
    // punto del digito actual
    point = screen->Current_Digit;
    // Si el parpadeo esta activo entra en el condicional
    if (screen->Flashing_Frecuency != 0) {
        if (screen->Current_Digit == 0) { // Si se trata del primer digito incrementa el contador
            screen->Flashing_Count = (screen->Flashing_Count + 1) % screen->Flashing_Frecuency;
        }
        // Si el contador es menor que la mitad de la frecuencia de parpadeo -> no se activa ningun segmento
        if (screen->Flashing_Count < (screen->Flashing_Frecuency / 2)) {
            /* code */
            if (screen->Current_Digit >= screen->Flashing_From && screen->Current_Digit <= screen->Flashing_To) {
                /* code */
                segments = 0;
            }
        }
    }
    // el valor de segments representa el numero a representar en el display
    screen->Driver.Segments_Turn_Update(segments);       // actualiza los segmentos
    screen->Driver.Digit_Turn_On(screen->Current_Digit); // enciende el digito actual

    // condicional para prender un punto constantemente
    if (screen->Point == screen->Current_Digit && screen->Point != screen->Flash_Point) {

        screen->Driver.Point_On();
    }

    // Condiconal para hacer parpadear un punto
    if (screen->Flash_Point == screen->Current_Digit && screen->Frecuency_Flashing_Point != 0) {
        if (screen->Flash_Point < screen->Digits) {
            // Condional para ver que no se asigno un digito mayor a la cantidad disponible
            screen->Flashing_Point_Count = (screen->Flashing_Point_Count + 1) % screen->Frecuency_Flashing_Point;
            screen->Driver.Point_On();
        }
        if (screen->Flashing_Point_Count < (screen->Frecuency_Flashing_Point / 2)) {
            point = 0;
            screen->Driver.Point_Off();
        }
    }

    // screen->Value[numeros a mostrar] si tengo 4 displays --> screen->Value [0,0,1,2]
    //  PrenderDigit(screen->Current_Digit); // podria pasarle un argumento que provoque que se apaguen todos
    //  Digit_Turn_On(screen->Current_Digit);
    //  estas funciones deberian estar en la interface
}
// Funcion de parpadeo
// frecuency es la frecuencia de parpadeo
int Display_Flash_Digits(screen_t screen, uint8_t from, uint8_t to, uint16_t frecuency) {
    int result = 0;
    if (from > to || from >= SCREEN_MAX_Digits || to >= SCREEN_MAX_Digits) {
        result = -1;
    } else if (!screen) {
        result = -1;
    } else {
        /* code */
        // Le asigno a los campos del objeto pantalla los parametros de la funcion
        screen->Flashing_Frecuency = frecuency * 2; // Multiplico por 2 para que la mitad del tiempo este en ON
        screen->Flashing_From = from;               // primer digito a parpadear
        screen->Flashing_To = to;                   // ultimo digito a parpadear
        screen->Flashing_Count = 0;                 // Contador de parpadeo inicializado en 0
        result = 0;                                 // retorna 0
    }
    return result;
}

// Funcion para cambair el punto que esta parpadeando
void Select_Point(screen_t screen, uint8_t digit) {
    screen->Point = digit;
}

// Funcion para asigar parametros de parpadeo al punto
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
// Funcion para prender el punto
// void Point_Turn_On(screen_t screen, uint8_t digit) {
//  if (digit < screen->Digits) {
//    screen->Driver.Point_On;
//}
//}
// Funcion para apagar el punto
// void Point_Turn_Off(screen_t screen, uint8_t digit) {
//  if (digit < screen->Digits) {
//     screen->Value[digit] &= ~SEGMENT_P; // Apagar el punto decimal sin cambiar los segmentos
//}
//}
/* === End of documentation ======================================================================================== */
