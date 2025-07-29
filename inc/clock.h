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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief Codigo fuente de clock.h
 @author Uberti, Ulises Leandro
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
/* === Public data type declarations =============================================================================== */

typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
} clock_time_t;

typedef struct clock_s * clock_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear el objeto reloj
 *
 * @param Ticks_Per_Second Cantidad de pulsos por segundo
 * @return clock_t puntero a la estructura creada
 */
clock_t Clock_Create(uint16_t Ticks_Per_Second);

/**
 * @brief Funcion para actulizar el estado de la hora del reloj
 *
 * @param clock objeto reloj
 * @param result esctructura con hora,minutos,segundos
 * @return true si es valida la hora
 * @return false si es invalida la hora
 */
bool Clock_Get_Time(clock_t clock, clock_time_t * result);

/**
 * @brief Funcion para obtener la hora del reloj
 *
 * @param Clock objeto reloj
 * @return clock_time_t un arreglo con la hora del reloj
 */
clock_time_t Clock_Time(clock_t Clock);

/**
 * @brief Funcion para cambiar la hora del reloj
 *
 * @param clock objeto reloj
 * @param new_time estructura con hora,minutos,segundos
 * @return true si es valida la hora ingresada
 * @return false si es valida la hora ingresada
 */
bool Clock_Set_Time(clock_t clock, clock_time_t * new_time);

/**
 * @brief Funcion para obtener los valores de los segmentos de los displays
 *
 * @param clock_time puntero al objeto reloj
 * @param value arreglo con los segmentos en BCD
 */
void Clock_Get_Displays_Values(clock_time_t * clock_time, uint8_t value[]);

/**
 * @brief Funcion para incrementar el valor de los minutos
 *
 * @param clock_time puntero a al arreglo con la hora
 */
void Clock_Increment_Minutes(clock_time_t * clock_time);

/**
 * @brief Funcion para decrementar el valor de los minutos
 *
 * @param clock_time puntero al arreglo de la hora
 */
void Clock_Decrement_Minutes(clock_time_t * clock_time);

/**
 * @brief Funcion para incrementar el valor de la hora
 *
 * @param clock_time puntero al arreglo de la hora
 */
void Clock_Increment_Hours(clock_time_t * clock_time);

/**
 * @brief Funcion para decrementar el valor de la hora
 *
 * @param clock_time puntero al arreglo de la hora
 */
void Clock_Decrement_Hours(clock_time_t * clock_time);

/**
 * @brief Funcion para contar un pulso
 *
 * @param clock objeto reloj
 */
void Clock_New_Tick(clock_t clock);

/**
 * @brief Funcion para setaer el horario de la alarma
 *
 * @param clock objeto reloj
 * @param alarm_time estructura con el horario de la alarma
 * @return true si se pudo setear la alarma
 * @return false si no se pudo setear la alarma
 */
bool Clock_Set_Time_Alarm(clock_t clock, clock_time_t * alarm_time);

/**
 * @brief Funcion para hacer sonar la alarma
 *
 * @param clock objeto reloj
 * @param alarm horario de la alarma seteada
 * @return true si la alarma sono
 * @return false si la alarma no sono
 */
bool Clock_Alarm_Working(clock_t clock, clock_time_t * alarm);

/**
 * @brief Funcion para apagar/prender la alarma
 *
 * @param clock objeto reloj
 * @param Encendida true si la alarma se activa y false si se desactiva
 * @return true si la alarma esta activa
 * @return false si la alarma esta desactivada
 */
bool Clock_Set_Alarm(clock_t clock, bool Encendida);

/**
 * @brief Funcion para setear el delay al posponer la alarma
 *
 * @param clock objeto alarma
 * @param delay_time minutos de delay
 * @return clock_time_t el horario de la alarma con delay
 */
clock_time_t Clock_Set_Alarm_Delay(clock_t clock, uint8_t delay_time);

/**
 * @brief Funcion para obtener la hora de la alarma
 *
 * @param clock objeto reloj
 * @return clock_time_t el horario de la alarma
 */
clock_time_t Clock_Alarm(clock_t clock);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */
