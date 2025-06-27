/*********************************************************************************************************************
Copyright (c) 2025, Uberti,Ulises Leandro <ubertileandro0@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file clock.c
 * @brief Código fuente de clock.c
 * @author Uberti, Ulises Leandro
 * */

/*
- La librería deberá mantener la hora actual, con precisión de segundos, a partir de la llamada
a una función que se produce una cierta cantidad de veces por segundo.
• Lalibrería deberá proporcionar una función para ajustar la hora actual.
• La librería deberá permitir la configuración, una vez al inicio, de la cantidad de veces por
segundo que se llamará a la función para mantener la hora actualizada.
• Lalibrería deberá informar que la hora actual es inválida hasta que se llama a la función para
ajustar la hora por primera vez.
• Lalibrería deberá proporcionar una función para fijar la hora de la alarma.
• Lalibrería deberá proporcionar una función para consultar la hora fijada para la alarma.
• Lalibrería deberá proporcionar una función para habilitar y deshabilitar la alarma.
• Lalibrería deberá proporcionar una función para consultar si la alarma está, o no, habilitada.
• La librería deberá generar un evento cuando la alarma esté habilitada y además hora actual
coincida con la hora de la alarma.
• La librería deberá proporcionar una función para posponer la alarma una cantidad arbitraria
de minutos.
• La librería deberá manejar todas las horas como un arreglo de bytes en formato BCD sin
compactar, con la decena de horas en la primera posición y la unidad de los segundos en la
última posición del vector
*/

/* === Headers files inclusions ====================================================================================*/
#include "clock.h"
#include <stddef.h>
#include <string.h>
/* === Macros definitions ==========================================================================================*/

/* === Private data type declarations==============================================================================*/

// Estructura con los atributos del objeto
struct clock_s {
    clock_time_t alarm;                 // Alarma seteada
    bool Alarm_Active;                  // estado de la alarma
    uint8_t delay;                      // delay al posponer alarma
    clock_time_t time_alarm_with_delay; // Alarma pospouesta
    bool Delay_Active;                  // Estado de la accion de posponer
    uint16_t Clock_Ticks;               // Cantidad de pulsos por segundo
    clock_time_t curren_time;           // Hora actual del reloj
    bool valid;                         // Hora valida del reloj
};

/* === Private function declarations ===============================================================================*/

/* === Private variable definitions================================================================================*/

/* === Public variable definitions =================================================================================*/

/* === Private function definitions ================================================================================*/

clock_t Clock_Create(uint16_t Ticks_Per_Second) {
    static struct clock_s clock[1];
    memset(clock, 0, sizeof(struct clock_s));
    clock->valid = false;        // Inicia en hora invalida
    clock->Alarm_Active = false; // Inicia con alarma desactivada
    clock->Delay_Active = false; // Inicia con delay desactivado
    return clock;
}

bool Clock_Set_Time(clock_t clock, clock_time_t * result) {
    // Reescribo los minutos,segundos y horas como numeros enteros
    uint8_t minutes = result->time.minutes[1] * 10 + result->time.minutes[0];
    uint8_t seconds = result->time.seconds[1] * 10 + result->time.seconds[0];
    uint8_t hours = result->time.hours[1] * 10 + result->time.hours[0];
    // Condicional para no ingresar un horario invalido
    if (hours < 24 && minutes < 60 && seconds < 60) {
        memcpy(&clock->curren_time, result, sizeof(clock_time_t));
        clock->valid = true;
    } else {
        // Si la hora es invalida seteo en 00:00:00
        clock_time_t current_time = {0};
        clock->curren_time = current_time;
        clock->valid = false;
    }
    // retorna una hora invalida o valida
    return clock->valid;
}

bool Clock_Get_Time(clock_t clock, clock_time_t * new_time) {
    //  memcpy para copiar los bytes del arreglo con la hora
    if (clock->valid) {
        memcpy(new_time, &clock->curren_time, sizeof(clock_time_t));
    }
    return clock->valid;
}

clock_time_t Clock_Time(clock_t Clock) {
    clock_time_t invalid = {0};
    if (Clock != NULL) {
        return Clock->curren_time;
    }
    return invalid;
}

void Clock_New_Tick(clock_t clock) {
    clock->curren_time.time.seconds[0]++;
    // Si el reloj esta por encima de 00:00:09 entonces
    if (clock->curren_time.time.seconds[0] > 9) {
        // regreso a 0 la unidad de segundos
        clock->curren_time.time.seconds[0] = 0;
        // aumento la decena de segundo en uno
        clock->curren_time.time.seconds[1]++;
    }
    // Si la decena de segundo es mayor a 5 entonces
    if (clock->curren_time.time.seconds[1] > 5) {
        // Nuevamente pongo en 0 la decena de segundo
        clock->curren_time.time.seconds[1] = 0;
        // Aumenta la unidad de minutos en 1
        clock->curren_time.time.minutes[0]++;
        // Si la unidad de minutos es mayor que 9 entonces
        if (clock->curren_time.time.minutes[0] > 9) {
            // Regreso a 0 la unidad de minutos
            clock->curren_time.time.minutes[0] = 0;
            // Aumenta en 1 la decena de minutos
            clock->curren_time.time.minutes[1]++;
        }
        // Si la decena de minuto es mayor que 5, entonces:
        if (clock->curren_time.time.minutes[1] > 5) {
            // Regreso a 0 la decena de minutos
            clock->curren_time.time.minutes[1] = 0;
            // aumento en uno la unidad de horas
            clock->curren_time.time.hours[0]++;
            if (clock->curren_time.time.hours[0] > 9) {
                clock->curren_time.time.hours[0] = 0;
                clock->curren_time.time.hours[1]++;
            }
            // si la hora se encuentra en 23 entonces
            if ((clock->curren_time.time.hours[1] > 2) ||
                (clock->curren_time.time.hours[0] > 3 && clock->curren_time.time.hours[1] == 2)) {
                clock->curren_time.time.hours[0] = 0;
                clock->curren_time.time.hours[1] = 0;
            }
        }
    }
}

bool Clock_Set_Time_Alarm(clock_t clock, clock_time_t * alarm_time) {
    bool result = false;
    uint8_t minutes = alarm_time->time.minutes[1] * 10 + alarm_time->time.minutes[0];
    uint8_t seconds = alarm_time->time.seconds[1] * 10 + alarm_time->time.seconds[0];
    uint8_t hours = alarm_time->time.hours[1] * 10 + alarm_time->time.hours[0];
    // Copio el horario de la alarma como atributo del reloj
    if (hours < 24 && minutes < 60 && seconds < 60) {
        memcpy(&clock->alarm, alarm_time, sizeof(clock_time_t));
        result = true;
    }

    // Retorno el estado de la hora de la alarma
    return result;
}

bool Clock_Alarm_Working(clock_t clock, clock_time_t * alarm) {
    // Incializo que la alarma no suene
    bool result = false;
    // Defino una variable del horario esperado de la alarma
    clock_time_t * expect_alarm = &clock->alarm;
    // Si se activo el delay el horario de la alarma cambia
    if (clock->Delay_Active) {
        expect_alarm = &clock->time_alarm_with_delay;
    }
    // Si la alarma esta activada y coiciden la hora del reloj con la hora esperada de la alarma entonces:
    if (memcmp(expect_alarm->bcd, clock->curren_time.bcd, sizeof(clock->alarm.bcd)) == 0 && clock->Alarm_Active) {
        result = true;
        // Desactivo el delay por que volvio a sonar
        clock->Delay_Active = false;
        // Regreso la alarma a su valor inicial para que vuelva a sonar a la misma hora al dia siguiente
        memcpy(&clock->time_alarm_with_delay, &clock->alarm, sizeof(clock_time_t));
    }
    // Retorno si la alarma sono o no
    return result;
}

bool Clock_Set_Alarm(clock_t clock, bool Encendida) {
    if (Encendida) {
        clock->Alarm_Active = true;
    } else {
        clock->Alarm_Active = false;
        clock->Delay_Active = false;
    }
    return clock->Alarm_Active;
}

clock_time_t Clock_Set_Alarm_Delay(clock_t clock, uint8_t delay_time) {
    // delay_time deben ser minutos
    clock->delay = delay_time;
    // Si es el primer delay
    if (clock->Delay_Active == false) {
        memcpy(&clock->time_alarm_with_delay, &clock->alarm, sizeof(clock_time_t));
    }
    uint8_t Units_Minutes = clock->time_alarm_with_delay.time.minutes[0];
    uint8_t Tens_Minutes = clock->time_alarm_with_delay.time.minutes[1];
    // Escribo los minutos totales
    uint8_t Total_Minutes = Units_Minutes + Tens_Minutes * 10 + delay_time;
    // Operador % permite obtener los minutos, si se pasa de 60 tambien obtiene correctamente
    uint8_t New_Minutes = Total_Minutes % 60;
    // Si se pasa de 60 habra una hora de diferencia o mas
    uint8_t Pass_Hours = Total_Minutes / 60;

    clock->time_alarm_with_delay.time.minutes[0] = New_Minutes % 10;
    clock->time_alarm_with_delay.time.minutes[1] = New_Minutes / 10;

    // hago los mismo para la hora
    uint8_t Units_Hours = clock->time_alarm_with_delay.time.hours[0];
    uint8_t Tens_Hours = clock->time_alarm_with_delay.time.hours[1];
    uint8_t Total_Hours = Units_Hours + Tens_Hours * 10 + Pass_Hours;
    uint8_t New_Hour = Total_Hours % 24;
    clock->time_alarm_with_delay.time.hours[0] = New_Hour % 10;
    clock->time_alarm_with_delay.time.hours[1] = New_Hour / 10;

    clock->Delay_Active = true;

    return clock->time_alarm_with_delay;
}

clock_time_t Clock_Alarm(clock_t clock) {
    clock_time_t alarm_time = {0};
    if (clock->Alarm_Active) {
        memcpy(alarm_time.bcd, &clock->alarm.bcd, sizeof(clock_time_t));
    }
    return alarm_time;
}

/* === Public function implementation ==============================================================================*/

/* === End of documentation========================================================================================*/
