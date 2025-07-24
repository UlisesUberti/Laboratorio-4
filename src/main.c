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
// Primero se incluye el freeRTOS
#include "FreeRTOS.h"
// Despues de haber incluido el freertos se incluye el archivo asociado a las tareas, colas, eventos..
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <stdbool.h>
#include "DigitalOut.h"
#include "DigitalIn.h"
#include "bsp.h"
#include "clock.h"
#include "ClockTask.h"

/* === Macros definitions ====================================================================== */

#define CANT_DISPLAYS 4
#define SW_0_EVENT ACCEPT    // Boton "Aceptar"
#define SW_1_EVENT CANCEL    // Boton "Cancelar"
#define SW_2_EVENT SET_TIME  // Boton "Setear Tiempo"
#define SW_3_EVENT SET_ALARM // Boton "Setear Alarma"
#define SW_4_EVENT DECREMENT // Boton "Decrementar valor"
#define SW_5_EVENT INCREMENT // Boton "Incrementar valor"

/* === Private data type declarations ========================================================== */

typedef enum {
    Clock_Init_Mode,
    Clock_Time_Mode,
    Clock_Set_Minutes_Mode,
    Clock_Set_Hours_Mode,
    Clock_Set_Alarm_Mode,
    Clock_Change_Time_Mode,
    Clock_Set_Minutes_Alarm_Mode,
    Clock_Set_Hours_Alarm_Mode,
} Clock_Mode_t;

static Board_t Board;

static clock_t Clock;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion para obtener los valores de los segmentos de los displays
 *
 * @param clock_time puntero al objeto reloj
 * @param value arreglo con los segmentos en BCD
 */
static void Clock_Get_Displays_Values(clock_time_t * clock_time, uint8_t value[]);

static bool Delay_Button(Digital_In_t Digital_In, uint32_t * start, uint32_t duration, bool * flag);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void Clock_Get_Displays_Values(clock_time_t * clock_time, uint8_t value[]) {
    value[0] = clock_time->time.hours[1];
    value[1] = clock_time->time.hours[0];
    value[2] = clock_time->time.minutes[1];
    value[3] = clock_time->time.minutes[0];
}

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
/* === Public function implementation ========================================================= */

int main(void) {
    // Ahora con freeRTOS vamos a separar las cosas en tareas
    //  -Actualizar la hora cada 1seg
    //  -Refrescar pantalla cada 1ms
    //  -Detectar la pulsacion de botones --> Utiliza Eventos
    //  Encender AlARMA --> utiliza Eventos
    //  Habra comunicacion por medio de colas entre la deteccion de botones y la pantalla
    //  Necesitamos el uso de mutex para la pantalla que es un recurso compartido
    //  Ya no es necesario el uso del Systick

    // Estructura con los punteros a las entradas y salidas digitales de la EDU-CIAA
    Board = Board_Create();
    // Creo el objeto Reloj
    Clock = Clock_Create(100);

    // Declaramos un puntero a una cola
    QueueHandle_t button_Queue;
    // Una cola es una estructura FIFO de datos --> 1ero en llegar es 1ero en salir
    // Para los botones necesitaremos saber cual se presiono y que tiempo

    // Declaramos un handle a un mutex
    SemaphoreHandle_t screen_Mutex;
    // Este mutex es la para la pantalla que se comparte entre el reloj y el refresco

    // Declaramos un grupo de eventos (handle al grupo de eventos)
    EventGroupHandle_t clock_Events;
    // El grupo de eventos es un conjunto de flags
    // Un evento seria la activacion de la alarma, cambio de modo del reloj, alarma pospuesta

    // Creo un grupo de eventos
    clock_Events = xEventGroupCreate();

    // Ahora creamos las tareas

    // Tarea "Clock" para el control de estados y actualizar hora cada 1seg
    xTaskCreate(ClockTask, "Clock", Clock_Task_Stack_Size, );

    // Variable para llevar la cuenta inicial de 1 segundo
    uint32_t last_time = 0;
    uint32_t refresh = 0;
    uint32_t inactivity_time = 0;

    // variable para incializar una sola vez el parpadeo
    bool init;
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
        switch (actual_mode) {

        case Clock_Init_Mode:
            if (!init) {
                // Todos los displays se inicializan parpadeando
                Display_Flash_Digits(Board->Screen, 0, 3, 200);
                // Se inicializa parpadeando el segundo punto
                Flash_Point(Board->Screen, 1, 1, 200);
                // incializo el parpadeo
                init = true;
            }

            // Escribo la pantalla
            Screen_Write_BCD(Board->Screen, value, 4);

            // Refresco cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }
            // Si no se presiona nada cuanto hasta que pasen 30 seg
            if (inactivity) {
                if (inactivity_time == 0) {
                    inactivity_time = Board_getMillis();
                } else if (Board_getMillis() - inactivity_time >= 30000) {
                    inactivity = false;
                    actual_mode = Clock_Time_Mode;
                    inactivity_time = 0;
                }
            }

            // Si se presiona Set_Time (F1) por mas de 3 seg pasamos al modo de configurar los minutos
            if (Delay_Button(Board->Set_Time, &F1_Delay, 3000, &Set_Time_flag)) {

                actual_mode = Clock_Set_Minutes_Mode;
                init = false;
                first_set = false;
                refresh = 0;
                F1_Delay = 0;
                Set_Time_flag = false;
            }

            break;

        case Clock_Set_Minutes_Mode:
            // hago parpadear los minutos
            if (!init) {
                Display_Flash_Digits(Board->Screen, 2, 3, 175);
                init = true;
            }

            // Si se presiona para incrementar el tiempo
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Minutes(&clock_Time);
            }

            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Minutes(&clock_Time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);

            // Refresco la pantalla cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Activated(Board->Accept)) {
                    __asm("NOP");
                }
                actual_mode = Clock_Set_Hours_Mode;
                init = false;
                refresh = 0;
            }

            // Si se presiona cancelar pasamos al modo inicial o al modo normal dependiendo de donde estabamos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Changed(Board->Cancel)) {
                    __asm("NOP");
                }
                if (first_set) {
                    actual_mode = Clock_Init_Mode;
                    first_set = false;

                } else {
                    actual_mode = Clock_Time_Mode;
                }
                refresh = 0;
                init = false;
            }

            break;

        case Clock_Set_Hours_Mode:
            // hago parpadear la hora
            if (!init) {
                Display_Flash_Digits(Board->Screen, 0, 1, 175);
                init = true;
            }

            // Si se presiona para incrementar la hora
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Hours(&clock_Time);
                while (Digital_In_Was_Changed(Board->Increment)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }
            // Si se presiona para decrementar la hora
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Hours(&clock_Time);
                while (Digital_In_Was_Changed(Board->Decrement)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);

            // Contador para pasar un segundo por interrupcion
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Activated(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                refresh = 0;
                init = false;
                actual_mode = Clock_Time_Mode;
                Clock_Set_Time(Clock, &clock_Time);
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Deactivated(Board->Cancel)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Minutes_Mode;
                refresh = 0;
                init = false;
            }
            break;

        case Clock_Time_Mode:
            if (!init) {
                init = true;
                // Parapadea el punto del 2do display cada 1s
                Flash_Point(Board->Screen, 1, 1, 1000);
                Display_Flash_Digits(Board->Screen, 0, 3, 0);
            }

            if (Board_getMillis() - last_time >= 1000) {
                // Actualizo last_time
                last_time = Board_getMillis();
                // Condicion para determinar si avanzo 1 seg
                Clock_New_Tick(Clock);
                // Actualizo la hora del reloj
                // Actualizo el valor de los displays
            }

            // Escribo la pantalla con la hora actualizada
            Clock_Get_Time(Clock, &clock_Time);
            Clock_Get_Displays_Values(&clock_Time, value);
            Screen_Write_BCD(Board->Screen, value, 4);

            // Refresco cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }

            // Si se presiona Set_Time pasamos a modificar los minutos
            if (Delay_Button(Board->Set_Time, &F1_Delay, 2000, &Set_Time_flag)) {
                actual_mode = Clock_Set_Minutes_Mode;
                init = false;
                refresh = 0;
                last_time = 0;
                F1_Delay = 0;
                Set_Time_flag = false;
            }

            // Si se preisona aceptar se activa la alarma
            if (Digital_In_Was_Activated(Board->Accept) && !alarm_sounding) {
                Clock_Set_Alarm(Clock, true);
                Select_Point_On(Board->Screen, 3);
            }

            // Si se presiona cancelar se desactiva la alarma
            if (Digital_In_Was_Activated(Board->Cancel) && !alarm_sounding) {
                Clock_Set_Alarm(Clock, false);
                All_Points_Off(Board->Screen);
                init = false;
            }

            //  Si la alarma esta activa y coincide el horario de la alarma con el del reloj se prende el led
            if (Clock_Alarm_Working(Clock, &alarm_time) && !alarm_sounding) {
                Digital_Out_Activate(Board->Led_3);
                alarm_sounding = true;
            }

            // Si suena la alarma y se presiona aceptar entonces se pospone 5 min
            if (Digital_In_Was_Activated(Board->Accept) && alarm_sounding) {
                Clock_Set_Alarm_Delay(Clock, 1);
                Digital_Out_Deactivate(Board->Led_3);
                alarm_sounding = false;
            }

            // Si suena la alarma y se presiona cancelar entonces se apaga hasta el otro dia
            if (Digital_In_Was_Activated(Board->Cancel) && alarm_sounding) {
                Clock_Set_Alarm(Clock, true);
                Digital_Out_Deactivate(Board->Led_3);
                alarm_sounding = false;
            }

            // Si se presiona el boton de alarma por mas de 3 segundos pasa al estado set_alarma
            if (Delay_Button(Board->Set_Alarm, &F2_Delay, 2000, &Set_Alarm_flag)) {
                actual_mode = Clock_Set_Alarm_Mode;
                init = false;
                refresh = 0;
                last_time = 0;
                F2_Delay = 0;
                Set_Alarm_flag = false;
            }

            break;

        case Clock_Set_Alarm_Mode:
            if (!init) {
                init = true;
                Display_Flash_Digits(Board->Screen, 0, 3, 0);
                All_Points_On(Board->Screen);
            }
            if (first_set_alarm) {
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                first_set_alarm = false;
            }

            // Muestro por pantalla el horario de la alarma
            Clock_Get_Displays_Values(&alarm_time, value);
            Screen_Write_BCD(Board->Screen, value, CANT_DISPLAYS);

            // Refresco cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                Screen_Refresh(Board->Screen);
                refresh = Board_getMillis();
            }

            // Si se presiona Set_Time se setea el horario de la alarma
            if (Digital_In_Was_Activated(Board->Set_Time)) {
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
                init = false;
                refresh = 0;
            }

            // Si se presiona el boton de alarma vuelve nuevamente a clock_time_mode
            if (Digital_In_Was_Activated(Board->Set_Alarm)) {
                actual_mode = Clock_Time_Mode;
                init = false;
                refresh = 0;
                All_Points_Off(Board->Screen);
            }
            break;

        case Clock_Set_Minutes_Alarm_Mode:
            // hago parpadear los minutos
            if (!init) {
                Display_Flash_Digits(Board->Screen, 2, 3, 200);
                init = true;
            }

            // Si se presiona para incrementar el tiempo
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Minutes(&alarm_time);
            }

            // Si se presiona para decrementar el tiempo
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Minutes(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Refresco la pantalla cada 1ms
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona Aceptar cambiamos al modo de configurar la hora
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Activated(Board->Accept)) {
                    __asm("NOP");
                }
                refresh = 0;
                actual_mode = Clock_Set_Hours_Alarm_Mode;
                init = false;
            }

            // Si se presiona cancelar pasamos al modo inicial o al modo normal dependiendo de donde estabamos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Changed(Board->Cancel)) {
                    __asm("NOP");
                }
                actual_mode = Clock_Set_Alarm_Mode;
                refresh = 0;
                init = false;
            }
            break;

        case Clock_Set_Hours_Alarm_Mode:
            // hago parpadear la hora
            if (!init) {
                Display_Flash_Digits(Board->Screen, 0, 1, 200);
                init = true;
            }

            // Si se presiona para incrementar la hora
            if (Digital_In_Was_Activated(Board->Increment)) {
                Clock_Increment_Hours(&alarm_time);
            }
            // Si se presiona para decrementar la hora
            if (Digital_In_Was_Activated(Board->Decrement)) {
                Clock_Decrement_Hours(&alarm_time);
            }

            // Actualizo el valor de los displays
            Clock_Get_Displays_Values(&alarm_time, alarm_value);
            Screen_Write_BCD(Board->Screen, alarm_value, 4);

            // Contador para pasar un segundo por interrupcion
            if (Board_getMillis() - refresh >= 1) {
                refresh = Board_getMillis();
                Screen_Refresh(Board->Screen);
            }

            // Si se presiona aceptar pasamos al modo de funcionamiento normal del reloj
            if (Digital_In_Was_Activated(Board->Accept)) {
                while (Digital_In_Was_Changed(Board->Accept)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                init = false;
                actual_mode = Clock_Time_Mode;
                Clock_Set_Time_Alarm(Clock, &alarm_time);
                refresh = 0;
                All_Points_Off(Board->Screen);
            }
            // Si es cancelar volvemos al modo de los minutos
            if (Digital_In_Was_Activated(Board->Cancel)) {
                while (Digital_In_Was_Deactivated(Board->Cancel)) {
                    __asm("NOP"); // instruccion para que no figure como vacio
                }
                actual_mode = Clock_Set_Minutes_Alarm_Mode;
                refresh = 0;
                init = false;
            }
            break;

        default:
            break;
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
