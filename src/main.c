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
#include "bsp.h"
#include "clock.h"
#include "DigitalOut.h"

// Archivo de la tarea del reloj
#include "ClockTask.h"
// Archivo de la tarea del refresco de pantalla
#include "RefreshTask.h"
// Archivo de Tick para controlar 1 seg de reloj
#include "TickTask.h"
// Archivo de la tarea para controlar la alarma
#include "AlarmTask.h"
// Archivo de la tarea para detectar los botones
#include "ButtonTask.h"

/* === Macros definitions ====================================================================== */

#define ACCEPT SW_0_EVENT    // Boton "Aceptar"
#define CANCEL SW_1_EVENT    // Boton "Cancelar"
#define SET_TIME SW_2_EVENT  // Boton "Setear Tiempo"
#define SET_ALARM SW_3_EVENT // Boton "Setear Alarma"
#define DECREMENT SW_4_EVENT // Boton "Decrementar valor"
#define INCREMENT SW_5_EVENT // Boton "Incrementar valor"

/* === Private data type declarations ========================================================== */

// Puntero a al objeto Placa
static Board_t Board;
// Puntero al objeto Reloj
static clock_t Clock;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion Baliza (prende y apaga un par de leds)
 *
 * @param args argumento de tarea del S.O.
 */
void Blinking(void * args);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

// Creamos una tarea de control
void Blinking(void * args) {
    while (true) {
        Digital_Out_Toggle(Board->Led_2);
        Digital_Out_Toggle(Board->Led_1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
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
    // Defino una variable que tome la hora con la que se inicializo el reloj
    // clock_time_t Init_Time = Clock_Time(Clock);

    // Declaramos un puntero a una cola
    // QueueHandle_t button_Queue;
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
    // Creo el mutex de la pantalla
    screen_Mutex = xSemaphoreCreateMutex();
    // Creamos una variable de control para determinar que las tareas se crearon correctamente
    BaseType_t result;

    // Ahora creamos las tareas

    // Si el mutex y el grupo de eventos se crearon entonces creamos la primera tarea
    if (screen_Mutex && clock_Events) {
        Tick_Task_Args_t Tick_Param = malloc(sizeof(*Tick_Param));
        Tick_Param->clock_events = clock_Events;
        result = xTaskCreate(Tick_Task, "Tick", Tick_Task_Stack_Size, Tick_Param, tskIDLE_PRIORITY + 5, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Refresh_Task_Args_t Refresh_Param = malloc(sizeof(*Refresh_Param));
        Refresh_Param->Board = Board;
        Refresh_Param->clock_events = clock_Events;
        Refresh_Param->screen_Mutex = screen_Mutex;
        result =
            xTaskCreate(Refresh_Task, "Refresh", Refresh_Task_Stack_Size, Refresh_Param, tskIDLE_PRIORITY + 4, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = ACCEPT;
        SW_Param->Switch = Board->Accept;
        result = xTaskCreate(Button_Task, "Accept", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = CANCEL;
        SW_Param->Switch = Board->Cancel;
        result = xTaskCreate(Button_Task, "Cancel", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = SET_TIME;
        SW_Param->Switch = Board->Set_Time;
        result = xTaskCreate(Button_Task, "SetTime", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = SET_ALARM;
        SW_Param->Switch = Board->Set_Alarm;
        result = xTaskCreate(Button_Task, "SetAlarm", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = DECREMENT;
        SW_Param->Switch = Board->Decrement;
        result = xTaskCreate(Button_Task, "Decrement", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Button_Task_Args_t SW_Param = malloc(sizeof(*SW_Param));
        SW_Param->Board = Board;
        SW_Param->clock_events = clock_Events;
        SW_Param->event_bit = INCREMENT;
        SW_Param->Switch = Board->Increment;
        result = xTaskCreate(Button_Task, "Increment", Button_Task_Stack_Size, SW_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Alarm_Task_Args_t Alarm_Param = malloc(sizeof(*Alarm_Param));
        Alarm_Param->Board = Board;
        Alarm_Param->clock_events = clock_Events;
        Alarm_Param->clock = Clock;
        result = xTaskCreate(Alarm_Task, "Alarm", Alarm_Task_Stack_Size, Alarm_Param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        Clock_Task_Args_t Clock_Param = malloc(sizeof(*Clock_Param));
        Clock_Param->clock = Clock;
        Clock_Param->Board = Board;
        Clock_Param->clock_Events = clock_Events;
        Clock_Param->screen_Mutex = screen_Mutex;
        result = xTaskCreate(Clock_Task, "Clock", Clock_Task_Stack_Size, Clock_Param, tskIDLE_PRIORITY + 2, NULL);
    }
    // Si alguna de las tareas no puede crearse ponemos una baliza
    if (result != pdPASS) {
        xTaskCreate(Blinking, "Baliza", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    }

    // Incializamos el Sistema Operativo (FreeRTOS)
    vTaskStartScheduler();
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
