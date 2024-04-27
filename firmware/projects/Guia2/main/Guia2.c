/*! @mainpage Guia2
 *
 * \section genDesc General Description
 * 
 * Esta aplicacion mide la distancia en cm utilizando un ultrasonido (HC-SR04). 
 * Ademas muestra por un display LCD lo medido e indica con leds, de diferentes colores, los valores dentros de los rangos establecidos. 
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
 *  
 * ----------------------------------------------------------------------------------------------------
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
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
/*==================[macros and definitions]=================================*/
#define MEASURE_PERIOD 1000
#define SWITCH_PERIOD 100

/*==================[internal data definition]===============================*/

bool hold = false;
bool on_off = false;
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/

/** @fn void Mostrar(void *pvParameter)
 * @brief Muestar por display LCD el valor medido y enciende el led correspondiente a la medicion 
 * @param 
 * @retval Null. 
*/
static void Mostrar(void *pvParameter)
{

    while (true)
    {
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

        vTaskDelay(MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}


/** @fn void Medir(void *pvParameter)
 * @brief Mide la distancia con una funcion del drive HC-SR04 
 * @param 
 * @retval Null. 
*/
static void Medir(void *pvParameter)
{
    while (true)
    {
        distancia = HcSr04ReadDistanceInCentimeters();
        vTaskDelay(MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}


/** @fn void Teclas(void *pvParameter)
 * @brief Lee el estado de las teclas 1 y 2 y cambia el estado de las banderas correspondietes a las mismas. 
 * @param  puntero a una funcion
 * @retval Null. 
*/
static void Teclas(void *pvParameter)
{
    int8_t tecla;

    while (true)
    {
        tecla = SwitchesRead();
        switch (tecla)
        {
        case SWITCH_1:
            on_off=!on_off;
            break;
        case SWITCH_2:
            hold=!hold;
            break;
        }

        vTaskDelay(SWITCH_PERIOD / portTICK_PERIOD_MS);
    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
    HcSr04Init(GPIO_3, GPIO_2);

    xTaskCreate(&Teclas, "Teclas_Estados", 512, NULL, 5, NULL);
    xTaskCreate(&Medir, "Medir", 2048, NULL, 5, NULL);
    xTaskCreate(&Mostrar, "Muestra_LCD", 512, NULL, 5, NULL);
}
