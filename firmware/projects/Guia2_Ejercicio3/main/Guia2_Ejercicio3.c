/*! @mainpage Guia2_Ejercicio3
 *
 * \section genDesc General Description
 *
 * This example makes LED_1, LED_2 and LED_3 blink at different rates, using FreeRTOS tasks.
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Magali Kristafor (magali.kristafor@)
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
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define MEASURE_PERIOD 1000000

/*==================[internal data definition]===============================*/

bool hold = false;
bool on_off = false;

uint16_t distancia = 0;

// son como las postas que se van pasando las tareas y el programa ppal. 
TaskHandle_t task_medir = NULL;
TaskHandle_t task_mostrar = NULL;

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
/*3 dígitos ascii + 1 carácter espacio + dos caracteres para la unidad (cm) + cambio de línea 
“ \r\n”*/
            UartSendString(UART_PC, UartItoa(distancia, 10));
            UartSendString(UART_PC, " (cm) \r\n")
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
        // vTaskDelay(MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}

static void FuncTimer(void *pvParameter)
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


void Control_Teclas(void)
{
    uint8_t dato;
    UartReadByte(UART_PC, &dato);
    switch (dato)
    {
    case 'o':
        interrupcion_Tecla1();
        break;
    case 'h':
        interrupcion_Tecla2();
        break;
    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

    // Inicializacion //
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

    /* Inicialización de puerto serie (UART) */
    serial_config_t uart_pc = {
        .port = UART_PC,
        .baud_rate = 115200,
        .func_p = Control_Teclas, /*!< Pointer to callback function to call when receiving data (= UART_NO_INT if not requiered)*/
        .param_p = NULL};
    UartInit(&uart_pc);

    xTaskCreate(&Medir, "Medir", 2048, NULL, 5, task_medir);
    xTaskCreate(&Mostrar, "Muestra_LCD", 512, NULL, 5, task_mostrar);
    TimerStart(timer_Measure.timer);
}
