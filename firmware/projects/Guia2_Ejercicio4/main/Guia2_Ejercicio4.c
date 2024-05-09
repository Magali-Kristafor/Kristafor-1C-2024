/*! @mainpage Guia2_Ejercicio4 (osciloscopio)
 *
 * \section genDesc General Description
 *
 * Esta aplicacion funciona con un osciloscopio.
 * Convierte una señal analogica en digital. 
 * Reconstruye una señal analogica apartir de un arreglo de datos (digital). 
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 24/04/2024 | Document creation		                         |
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
#include "timer_mcu.h"
#include "led.h"
#include "analog_io_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
#define PERIOD_SAMPLE 2000
#define PERIOD_ECG 4000
#define BUFFER_SIZE 231
/*==================[internal data definition]===============================*/

TaskHandle_t AD_task = NULL;
TaskHandle_t DA_ECG_task = NULL;

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};
/*==================[internal functions declaration]=========================*/

/**
 * @brief Función invocada en la interrupción del timer A. 
 */
void FuncTimerA(void *param)
{
    
    vTaskNotifyGiveFromISR(AD_task, pdFALSE); /* Envía una notificación a la tarea asociada a LeerDatos_AD */
}

/**
 * @brief Función invocada en la interrupción del timer B. 
 */
void FuncTimerB(void *param) 
{
    vTaskNotifyGiveFromISR(DA_ECG_task, pdFALSE); /* Envía una notificación a la tarea asociada a Convert_DA_ECG */
}

/**
 * @brief Tarea encargada de leer datos analogicos, potenciometro. (analog-->digital) 
 */
static void LeerDatos_AD(void *pvParameter)
{
    uint16_t valor;
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        AnalogInputReadSingle(CH1, &valor);
        UartSendString(UART_PC, (char *)UartItoa(valor, 10));
        UartSendString(UART_PC, "\r\n");
    }
}

/**
 * @brief Tarea encargada de convertir un arreglo de datos en una grafica de ecg. (digital-->analog)
 */
static void Convert_DA_ECG(void *pvParameter)
{
    uint8_t i = 0;
    
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        AnalogOutputWrite(ecg[i]);
        i++;

        if (i > BUFFER_SIZE)
        {
            i = 0;
        }
        
    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

    /* Inicialización de timers */
    timer_config_t timer_AD = {
        .timer = TIMER_A,
        .period = PERIOD_SAMPLE,
        .func_p = FuncTimerA,
        .param_p = NULL};
    TimerInit(&timer_AD);

    timer_config_t timer_DA_ECG = {
        .timer = TIMER_B,
        .period = PERIOD_ECG,
        .func_p = FuncTimerB,
        .param_p = NULL};
    TimerInit(&timer_DA_ECG);

    /* Inicialización del Puerto serie (UART)*/
    serial_config_t Puerto_Serie = {
        .port = UART_PC,
        .baud_rate = 115200,
        .func_p = NULL,
        .param_p = NULL};
    UartInit(&Puerto_Serie);

    /* Inicialización AD inputs */
    analog_input_config_t AD = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .sample_frec = NULL,
        .param_p = NULL};

    AnalogInputInit(&AD);
    AnalogOutputInit();

    /* Creación de tareas */
    xTaskCreate(&LeerDatos_AD, "Convert_AD", 512, NULL, 5, &AD_task);
    xTaskCreate(&Convert_DA_ECG, "Convert_DA", 512, NULL, 5, &DA_ECG_task);

    /* Inicialización del conteo de timers */
    TimerStart(timer_AD.timer);
    TimerStart(timer_DA_ECG.timer);

}
