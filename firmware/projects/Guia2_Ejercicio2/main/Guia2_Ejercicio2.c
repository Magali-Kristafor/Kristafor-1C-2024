/*! @mainpage Guia2_Ejercicio2
 *
 * \section genDesc General Description
 * 
 * Esta aplicacion mide la distancia en cm utilizando un ultrasonido (HC-SR04). 
 * Ademas muestra por un display LCD lo medido e indica con leds de diferentes colores los valores dentros de los rangos establecidos. 
 *  
 * Rangos 
 *      Distancia (cm)        LEDs
 *      [0-10]               Ninguno
 *      [10-20]              LED_1
 *      [20-30]              LED_2
 *       >30                 LED_3
 * 
 * Se enciende y apaga la aplicacion al presionar tecla 1. AL presionar tecla 2 holdea el valor medido. 
 *
 *  // Se implenta la aplicacion utilizando interrupciones y timers //
 * 
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 10/04/2024 | Document creation		                         |
 *
 * @author Magali Kristafor (magali.kristafor@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "hc_sr04.h"
#include "switch.h"
#include "lcditse0803.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/
#define MEASURE_PERIOD 1000000
#define SWITCH_PERIOD 100000

/*==================[internal data definition]===============================*/

bool hold = false;
bool on_off = false;

uint16_t distancia = 0;

// banderas para las tareas //

TaskHandle_t task_medir= NULL;
TaskHandle_t task_mostrar= NULL;

/*==================[internal functions declaration]=========================*/

static void Mostrar(void *pvParameter)
{

    while (true)
    {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (on_off)
        {
            if ((distancia >= 0) && (distancia <= 10))
            {
                LedOff(LED_1);
                LedOff(LED_2);
                LedOff(LED_3);
            }
            if ((distancia > 10) && (distancia <= 20))
            {
                LedOn(LED_1);
                LedOff(LED_2);
                LedOff(LED_3);
            }
            if ((distancia > 20) && (distancia <= 30))
            {
                LedOn(LED_1);
                LedOn(LED_2);
                LedOff(LED_3);
            }
            if (distancia > 30)
            {
                LedOn(LED_1);
                LedOn(LED_2);
                LedOn(LED_3);
            }
            if (!hold)
            {
                LcdItsE0803Write(distancia);
            }
        }
        else
        {
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
            LcdItsE0803Off();
        }

       
    }
}

static void Medir(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        distancia = HcSr04ReadDistanceInCentimeters();
        //vTaskDelay(MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}

static void FuncTimer (void *pvParameter)
{
    xTaskNotifyGive(task_medir);
    xTaskNotifyGive(task_mostrar);
}

void interrupcion_Tecla1(void)
{
    on_off = !on_off;
}

void interrupcion_Tecla2(void)
{
    hold = !hold;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
    HcSr04Init(GPIO_3, GPIO_2);

    SwitchActivInt(SWITCH_1, &interrupcion_Tecla1);

    SwitchActivInt(SWITCH_2, &interrupcion_Tecla2);

        /* Inicialización de timers */
    timer_config_t timer_Measure = {
        .timer = TIMER_A,
        .period = MEASURE_PERIOD,
        .func_p = FuncTimer,
        .param_p = NULL};
    TimerInit(&timer_Measure);

    xTaskCreate(&Medir, "Medir", 2048, NULL, 5, task_medir);
    xTaskCreate(&Mostrar, "Muestra_LCD", 512, NULL, 5, task_mostrar);
    TimerStart(timer_Measure.timer);
}
