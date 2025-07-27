/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @brief Codigo fuente del archivo main.c
 ** @author Uberti, Ulises Leandro
 ** @file main.c
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include "DigitalOut.h"
#include "DigitalIn.h"
#include "bsp.h"
#include "clock.h"

/* === Macros definitions ====================================================================== */

// Cantidad de dsiplays
#define CANT_DISPLAYS 4

// Tiempo para evitar rebote mecanico
#define DEBOUNCE_TIME 20

/* === Private data type declarations ========================================================== */

typedef enum {
    Clock_Init_Mode,
    Clock_Time_Mode,
    Clock_Set_Minutes_Mode,
    Clock_Set_Hours_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Set_Minutes_Alarm_Mode,
    Clock_Set_Hours_Alarm_Mode,
} Clock_Mode_t;

/* === Private variable declarations =========================================================== */

// Estructura para evitar el rebote del boton aceptar
typedef struct {
    // variable para almacenar el tiempo en que el boton se presiono
    uint32_t button_time;
} Debounce_t;

// Puntero a la placa
static Board_t Board;
// Puntero al reloj
static clock_t Clock;
// puntero al arreglo con la hora
static Clock_Mode_t actual_mode;

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion para detectar que un boton se mantuvo presionado x [ms]
 *
 * @param Digital_In Entrada digital
 * @param start tiempo de inicio del contador (en cero)
 * @param duration tiempo que debe estar presionado
 * @param flag bandera que indica si se presiono o no
 * @return true Si se mantuvo presionado el boton x [ms]
 * @return false si no se mantuvo presionado el boton x[ms]
 */
static bool Delay_Button(Digital_In_t Digital_In, uint32_t * start, uint32_t duration, bool * flag);

/**
 * @brief Funcion para evitar el rebote de un boton
 *
 * @param Digital_In entrada digital (boton)
 * @param button puntero a la estructura del boton correspondiente
 * @return true si pasaron 10ms desde que se dejo de presionar el boton
 * @return false si no pasaron los 10ms desde que se dejo de presionar el boton
 */
static bool Button_Debounce(Digital_In_t Digital_In, Debounce_t * button);

/**
 * @brief Funcion para cambiar el estado en que se encuentra el reloj
 *
 * @param Mode Modo al que se quiere pasar
 * @return Clock_Mode_t retorna el modo al que se paso
 */
static void Change_Mode(Clock_Mode_t Mode);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static bool Delay_Button(Digital_In_t Digital_In, uint32_t * start, uint32_t duration, bool * flag) {
    if (Digital_In_GetState(Digital_In)) {
        if (!(*flag)) {
            *start = Board_getMillis();
            *flag = true;
        } else if (Board_getMillis() - *start >= duration) {
            *start = 0;
            *flag = false;
            return true;
        }
    } else {
        *start = 0;
        *flag = false;
    }
    return false;
}

static bool Button_Debounce(Digital_In_t Digital_In, Debounce_t * button) {
    uint32_t now;
    if (Digital_In_Was_Activated(Digital_In)) {
        now = Board_getMillis();
        // Si el tiempo desde que se pulso el boton es mayor el tiempo para evitar rebote entonces se permite la
        // pulsacion
        if (now - button->button_time >= DEBOUNCE_TIME) {
            button->button_time = now;
            return true;
        }
    }
    // No permite la pulsacion del boton
    return false;
}

void Change_Mode(Clock_Mode_t Mode) {
    actual_mode = Mode;
    switch (Mode) {
    case Clock_Init_Mode:
        Display_Flash_Digits(Board->Screen, 0, 3, 200);
        Flash_Point(Board->Screen, 1, 1, 200);
        break;
    case Clock_Set_Minutes_Mode:
        Display_Flash_Digits(Board->Screen, 2, 3, 175);
        break;
    case Clock_Set_Hours_Mode:
        Display_Flash_Digits(Board->Screen, 0, 1, 175);
        break;
    case Clock_Time_Mode:
        Flash_Point(Board->Screen, 1, 1, 1000);
        Display_Flash_Digits(Board->Screen, 0, 3, 0);
        break;
    case Clock_Set_Alarm_Mode:
        Display_Flash_Digits(Board->Screen, 0, 3, 0);
        All_Points_On(Board->Screen);
        break;
    case Clock_Set_Minutes_Alarm_Mode:
        Display_Flash_Digits(Board->Screen, 2, 3, 200);
        break;
    case Clock_Set_Hours_Alarm_Mode:
        Display_Flash_Digits(Board->Screen, 0, 1, 200);
        break;

    default:
        break;
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    // Estructura con los punteros a las entradas y salidas digitales de la EDU-CIAA
    Board = Board_Create();
    // Creo el objeto Reloj
    Clock = Clock_Create(100);
    // Incializo Tick
    Init_Tick();
    // Defino un puntero a la hora del reloj
    clock_time_t clock_Time = Clock_Time(Clock);
    // Defino un arreglo para almacenar el valor de los displays
    uint8_t value[4] = {0};
    // Defino un arreglo para almacenar el valor de los displays con la alarma
    uint8_t alarm_value[4] = {0};
    // Defino un estado incial del reloj
    Change_Mode(Clock_Init_Mode);
    // Defino un horario de alarma por defecto
    clock_time_t alarm_time = {0};
    // Defino un puntero que guarde la direccion de la hora mientras esta en otro proceso
    // clock_time_t clock_actual_time;
    // Puntero a los botones que necesitan antirebote
    Debounce_t Accept = {0};
    Debounce_t Cancel = {0};
    Debounce_t Increment = {0};
    Debounce_t Decrement = {0};
    // Variable para llevar la cuenta inicial de 1 segundo
    uint32_t last_time = 0;
    uint32_t refresh = 0;
    uint32_t inactivity_time = 0;

    // Variable para detectar inactividad
    bool inactivity = true;
    //  Variable que avisa si es la primera inicializacion de tiempo
    bool first_set = true;

    // variable para indicar que la alarma cambio su horario por defecto
    bool first_set_alarm = true;
    //
    bool Set_Time_flag = false;
    uint32_t F1_Delay = 0;

    bool Set_Alarm_flag = false;
    uint32_t F2_Delay = 0;

    bool alarm_sounding = false;

    while (true) {
        if (actual_mode == Clock_Init_Mode) {
            // Si no se setea el tiempo hasta que pasen 30 seg
            if (inactivity) {
                if (inactivity_time == 0) {
                    inactivity_time = Board_getMillis();
                } else if (Board_getMillis() - inactivity_time >= 30000) {
                    inactivity = false;
                    Change_Mode(Clock_Time_Mode);
                    inactivity_time = 0;
                }
            }
            // Si se presiona Set_Time (F1) por mas de 3 seg pasamos al modo de configurar los minutos
            if (Delay_Button(Board->Set_Time, &F1_Delay, 3000, &Set_Time_flag)) {
                Change_Mode(Clock_Set_Minutes_Mode);
                first_set = false;
                F1_Delay = 0;
                Set_Time_flag = false;
                inactivity = true;
                inactivity_time = 0;
            }
        } else if (actual_mode == Clock_Set_Minutes_Mode) {

            // Si se presiona para incrementar el tiempo
            if (Button_Debounce(Board->Increment, &Increment)) {
                Clock_Increment_Minutes(&clock_Time);
            }

            // Si se presiona para decrementar el tiempo
            if (Button_Debounce(Board->Decrement, &Decrement)) {
                Clock_Decrement_Minutes(&clock_Time);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Button_Debounce(Board->Accept, &Accept)) {
                // espero 10ms
                // cambio de estado y reinicio banderas
                Change_Mode(Clock_Set_Hours_Mode);
            }

            // Si se presiona cancelar pasamos al modo inicial o al modo normal dependiendo de donde estabamos
            if (Button_Debounce(Board->Cancel, &Cancel)) {

                // Si se cancela siendo la primera vez, entonces vuelve al modo inicial
                if (first_set) {
                    Change_Mode(Clock_Init_Mode);
                    first_set = false;
                } else {
                    Change_Mode(Clock_Time_Mode);
                }
            }
        } else if (actual_mode == Clock_Set_Hours_Mode) {
            // Si se presiona para incrementar la hora
            if (Button_Debounce(Board->Increment, &Increment)) {
                Clock_Increment_Hours(&clock_Time);
            }
            // Si se presiona para decrementar la hora
            if (Button_Debounce(Board->Decrement, &Decrement)) {
                Clock_Decrement_Hours(&clock_Time);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Button_Debounce(Board->Accept, &Accept)) {
                // espero 10ms
                Change_Mode(Clock_Time_Mode);
                // Seteo el nuevo horario
                Clock_Set_Time(Clock, &clock_Time);
            }

            // Si es cancelar volvemos al modo de los minutos
            if (Button_Debounce(Board->Cancel, &Cancel)) {
                // espero 10ms
                Change_Mode(Clock_Set_Minutes_Mode);
            }
        } else if (actual_mode == Clock_Time_Mode) {

            if (Board_getMillis() - last_time >= 1000) {
                // Actualizo last_time
                last_time = Board_getMillis();
                // Condicion para determinar si avanzo 1 seg
                Clock_New_Tick(Clock);
            }

            // Si se presiona Set_Time por 3seg pasamos a modificar los minutos
            if (Delay_Button(Board->Set_Time, &F1_Delay, 2000, &Set_Time_flag)) {
                Change_Mode(Clock_Set_Minutes_Mode);
                last_time = 0;
                F1_Delay = 0;
                Set_Time_flag = false;
            }

            // Si se preisona aceptar se activa la alarma
            if (Button_Debounce(Board->Accept, &Accept) && !alarm_sounding) {
                // espero 10ms
                Clock_Set_Alarm(Clock, true);
                Select_Point_On(Board->Screen, 3);
            }

            // Si se presiona cancelar se desactiva la alarma
            if (Button_Debounce(Board->Cancel, &Cancel) && !alarm_sounding) {
                // espero 10ms
                Clock_Set_Alarm(Clock, false);
                All_Points_Off(Board->Screen);
            }

            //  Si la alarma esta activa y coincide el horario de la alarma con el del reloj se prende el led
            if (Clock_Alarm_Working(Clock, &alarm_time) && !alarm_sounding) {
                // espero 10ms
                Digital_Out_Activate(Board->Led_3);
                alarm_sounding = true;
            }

            // Si suena la alarma y se presiona aceptar entonces se pospone 5 min
            if (Button_Debounce(Board->Accept, &Accept) && alarm_sounding) {
                // espero 10ms
                Clock_Set_Alarm_Delay(Clock, 1);
                Digital_Out_Deactivate(Board->Led_3);
                alarm_sounding = false;
            }

            // Si suena la alarma y se presiona cancelar entonces se apaga hasta el otro dia
            if (Button_Debounce(Board->Cancel, &Cancel) && alarm_sounding) {
                // espero 10ms
                // Clock_Set_Alarm(Clock, true);
                Digital_Out_Deactivate(Board->Led_3);
                alarm_sounding = false;
            }

            // Si se presiona el boton de alarma por mas de 3 segundos pasa al estado set_alarma
            if (Delay_Button(Board->Set_Alarm, &F2_Delay, 2000, &Set_Alarm_flag)) {
                Change_Mode(Clock_Set_Alarm_Mode);
                last_time = 0;
                F2_Delay = 0;
                Set_Alarm_flag = false;
            }
        } else if (actual_mode == Clock_Set_Alarm_Mode) {
            if (first_set_alarm) {
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                first_set_alarm = false;
            }

            // Si se presiona Set_Time se setea el horario de la alarma
            if (Digital_In_Was_Activated(Board->Set_Time)) {
                Change_Mode(Clock_Set_Minutes_Alarm_Mode);
            }

            // Si se presiona el boton de alarma vuelve nuevamente a clock_time_mode
            if (Digital_In_Was_Activated(Board->Set_Alarm)) {
                Change_Mode(Clock_Time_Mode);
                All_Points_Off(Board->Screen);
            }
        } else if (actual_mode == Clock_Set_Minutes_Alarm_Mode) {

            // Si se presiona para incrementar el tiempo
            if (Button_Debounce(Board->Increment, &Increment)) {
                Clock_Increment_Minutes(&alarm_time);
            }

            // Si se presiona para decrementar el tiempo
            if (Button_Debounce(Board->Decrement, &Decrement)) {
                Clock_Decrement_Minutes(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Button_Debounce(Board->Accept, &Accept)) {
                // espero 10ms
                Change_Mode(Clock_Set_Hours_Alarm_Mode);
            }

            // Si se presiona cancelar pasamos al modo normal dependiendo de donde estabamos
            if (Button_Debounce(Board->Cancel, &Cancel)) {
                Change_Mode(Clock_Set_Alarm_Mode);
            }
        } else if (actual_mode == Clock_Set_Hours_Alarm_Mode) {
            // Si se presiona para incrementar la hora
            if (Button_Debounce(Board->Increment, &Increment)) {
                Clock_Increment_Hours(&alarm_time);
            }
            // Si se presiona para decrementar la hora
            if (Button_Debounce(Board->Decrement, &Decrement)) {
                Clock_Decrement_Hours(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Button_Debounce(Board->Accept, &Accept)) {
                // espero 10ms
                Change_Mode(Clock_Time_Mode);
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                All_Points_Off(Board->Screen);
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Button_Debounce(Board->Cancel, &Cancel)) {
                Change_Mode(Clock_Set_Minutes_Alarm_Mode);
            }
        }

        // Refresco cada 1ms
        if (Board_getMillis() - refresh >= 1) {
            Screen_Refresh(Board->Screen);
            refresh = Board_getMillis();
        }
        // Escribo la pantalla
        // Actualizo el valor de los displays
        if ((actual_mode == Clock_Set_Minutes_Mode) || (actual_mode == Clock_Set_Hours_Mode)) {
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);
        } else if ((actual_mode == Clock_Time_Mode)) {
            // Escribo la pantalla con la hora actualizada
            Clock_Get_Time(Clock, &clock_Time);
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
