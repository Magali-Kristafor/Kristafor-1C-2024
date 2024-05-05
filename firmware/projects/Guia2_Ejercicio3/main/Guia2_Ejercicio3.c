/*! @mainpage Guia2_Ejercicio3
 *
 * \section genDesc General Description
 *
 *  Esta aplicacion mide la distancia en cm utilizando un ultrasonido (HC-SR04). 
 * Ademas muestra por un display LCD lo medido e indica con leds de diferentes colores los valores dentro de los rangos establecidos. 
 *  
 * Rangos 
 *      Distancia (cm)        LEDs
 *      [0-10]               Ninguno
 *      [10-20]              LED_1
 *      [20-30]              LED_2
 *       >30                 LED_3
 * 
 * Se enciende y apaga la aplicacion al presionar tecla 'o' del teclado de la compu.
 * Al presionar tecla 'h' del teclado de la compu, holdea el valor medido. 
 *
 *  // Se implenta la aplicacion utilizando interrupciones, timers e interfaz por puerto serie (UART) //
 * 
 * * @section hardConn Hardware Connection
 *
 * |    Peripheral  |           	|
 * |    ultrasonido |     ESP32     |
 * |    (HC-SR04)   |               |
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	ECHO	 	| 	GPIO_2		|
 * | 	TRIGGER	 	| 	GPIO_3		|
 * |    +5V         |   +5V         |
 * |    GND         |   GND         |

 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 17/04/2024 | Document creation		                         |
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


/** @brief Tarea encargada de mostar por display LCD el valor medido y encender el led correspondiente a la medicion 
*/
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


/** @brief Tarea encargada de medir la distancia.
*/
static void Medir(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        distancia = HcSr04ReadDistanceInCentimeters();
        // vTaskDelay(MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}


/** @brief Función invocada en la interrupción del timer
 */
static void FuncTimer(void *pvParameter)
{
    xTaskNotifyGive(task_medir);
    xTaskNotifyGive(task_mostrar);
}


/** @fn void interrupcion_Tecla1(void)
  @brief Interrupcion asociada a la tecla 1. Cambia su estado(on/off) en el caso de ser presionada. 
  @param[in] null
  @retval null
 */
void interrupcion_Tecla1(void)
{
    on_off = !on_off;
}


/** @fn void interrupcion_Tecla2(void)
  @brief Interrupcion asociada a la tecla 2. Cambia su estado(hold/!hold) en el caso de ser presionada. 
  @param[in] null
  @retval null
 */
void interrupcion_Tecla2(void)
{
    hold = !hold;
}


/** @fn void Control_Teclas(void)
  @brief Interrupcion para el control de teclas, h; o,  asociadas a la interfase en serie.
  @param[in] null
  @retval null
 */
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

    /*Interrupcion de teclas*/
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

    /*Creacion de tareas*/
    xTaskCreate(&Medir, "Medir", 2048, NULL, 5, task_medir);
    xTaskCreate(&Mostrar, "Muestra_LCD", 512, NULL, 5, task_mostrar);

    /*Inicio del timer*/
    TimerStart(timer_Measure.timer);
}
