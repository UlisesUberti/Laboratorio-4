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

// Incluimos tareas
#include "task.h"
// Incluimos mutex
#include "semphr.h"
// Incluimos eventos
#include "event_groups.h"

#include <stdbool.h>
#include "bsp.h"
#include "clock.h"
#include "DigitalOut.h"
#include <stdlib.h>

// Archivo de la tarea del reloj
#include "ClockTask.h"
// Archivo de la tarea del refresco de pantalla
#include "RefreshTask.h"
// Archivo de Tick para controlar 1 seg de reloj
#include "TickTask.h"
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
static void Blinking(void * args);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void Blinking(void * args) {
    while (true) {
        Digital_Out_Toggle(Board->Led_2);
        Digital_Out_Toggle(Board->Led_1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    // Puntero a la estructura con los punteros a las entradas y salidas digitales de la EDU-CIAA
    Board = Board_Create();
    // Puntero al objeto Reloj
    Clock = Clock_Create(100);

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

    // Si el mutex y el conjunto de eventos se creo sin problema entonces creamos la primera tarea
    if (screen_Mutex && clock_Events) {
        // Tarea de refresco, maxima prioridad
        Refresh_Task_Args_t Refresh_Param = malloc(sizeof(*Refresh_Param));
        Refresh_Param->Board = Board;
        Refresh_Param->clock_events = clock_Events;
        Refresh_Param->screen_Mutex = screen_Mutex;
        result =
            xTaskCreate(Refresh_Task, "Refresh", Refresh_Task_Stack_Size, Refresh_Param, tskIDLE_PRIORITY + 4, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de Tick
        Tick_Task_Args_t Tick_Param = malloc(sizeof(*Tick_Param));
        Tick_Param->clock_events = clock_Events;
        Tick_Param->tick_event = TICK_1_SECOND_EVENT;
        Tick_Param->Board = Board;
        result = xTaskCreate(Tick_Task, "Tick", Tick_Task_Stack_Size, Tick_Param, tskIDLE_PRIORITY + 3, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea del Reloj
        Clock_Task_Args_t Clock_Param = malloc(sizeof(*Clock_Param));
        Clock_Param->clock = Clock;
        Clock_Param->Board = Board;
        Clock_Param->clock_Events = clock_Events;
        Clock_Param->screen_Mutex = screen_Mutex;
        result = xTaskCreate(Clock_Task, "Clock", Clock_Task_Stack_Size, Clock_Param, tskIDLE_PRIORITY + 4, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Aceptar"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_short_bit = ACCEPT;
        Sw_param->Switch = Board->Accept;
        result = xTaskCreate(Button_Short_Task, "Accept", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Cancelar"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_short_bit = CANCEL;
        Sw_param->Switch = Board->Cancel;
        result = xTaskCreate(Button_Short_Task, "Cancel", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Setear Alarma"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_long_bit = SW_LONG_3_EVENT;
        Sw_param->Switch = Board->Set_Alarm;
        result =
            xTaskCreate(Button_Long_Task, "Set_Alarm", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Incrementar"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_short_bit = INCREMENT;
        Sw_param->Switch = Board->Increment;
        result =
            xTaskCreate(Button_Short_Task, "Increment", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Decrementar"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_short_bit = DECREMENT;
        Sw_param->Switch = Board->Decrement;
        result =
            xTaskCreate(Button_Short_Task, "Decrement", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si la tarea anterior se creo sin problema entonces creamos la siguiente
    if (result == pdPASS) {
        // Tarea de tecla "Setear tiempo"
        Button_Task_Args_t Sw_param = malloc(sizeof(*Sw_param));
        Sw_param->clock_events = clock_Events;
        Sw_param->event_long_bit = SW_LONG_EVENT;
        Sw_param->Switch = Board->Set_Time;
        result =
            xTaskCreate(Button_Long_Task, "Set_Time", Button_Task_Stack_Size, Sw_param, tskIDLE_PRIORITY + 1, NULL);
    }
    // Si alguna de las tareas no puede crearse ponemos una baliza
    if (result != pdPASS) {
        // Tarea de control
        xTaskCreate(Blinking, "Baliza", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    }

    // Incializamos el Sistema Operativo (FreeRTOS)
    vTaskStartScheduler();
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
