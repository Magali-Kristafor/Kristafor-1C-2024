/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * Esta aplicacion permite controlar un sistemas de riego de agua y de unas soluciones basicas y acidas.
 *
 * * @section hardConn Hardware Connection
 *

 * |    Bombas          |   ESP32   	|
 * |:-----------------: |:--------------|
 * | 	PhA 	 	  	| 	GPIO_20		|
 * | 	PhB 	 		| 	GPIO_21		|
 * | 	Agua	 		| 	GPIO_22		|
 *
 *
 * |    Sensor Humedad  |   ESP32   	|
 * |:-----------------: |:--------------|
 * | 	PIN 	 	  	| 	GPIO_23		|

 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/06/2024 | Document creation		                         |
 *
 * @author  Magali Kristafor (magali.kristafor@ingenieria.uner.edu.ar)
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
#include "gpio_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "switch.h"

/*==================[macros and definitions]=================================*/
#define PERIOD_PH_HUMEDAD 3000000 // 3 s
#define PERIOD_UART 5000000       // 5s
/*==================[internal data definition]===============================*/
TaskHandle_t task_ph = NULL;
TaskHandle_t task_humedad = NULL;
TaskHandle_t task_comunicacion = NULL;

// Variables para la lectura
uint16_t valor_ph_dig;
float nivel_ph;

// variables para teclas
bool flg_on = false;
bool flg_off = false;

// Variables para el sensor de humedad
bool sensor_humedad = false;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void *param)
{
    /* Envía una notificación a la tarea asociada a controlar el PH y el Agua  */
    vTaskNotifyGiveFromISR(task_ph, pdFALSE);
    vTaskNotifyGiveFromISR(task_humedad, pdFALSE);
}

/**
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void *param)
{
    vTaskNotifyGiveFromISR(task_comunicacion, pdFALSE);
    ; /* Envía una notificación a la tarea asociada a la comunicacion por UART */
}

/**
 * @brief Tarea encargada encargada de controlar el PH
 */
static void Control_PH(void *pvParameter)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        if (flg_on)
        {
            AnalogInputReadSingle(CH1, &valor_ph_dig);
            nivel_ph = (14 / 3) * valor_ph_dig;

            if (nivel_ph > 6.7)
            {
                GPIOOn(GPIO_20); // bomba PhA
            }

            if (nivel_ph < 6.0)
            {
                GPIOOn(GPIO_21); // bomba PhBasica
            }
        }
        if (flg_off)
        {
            GPIOOff(GPIO_21);
            GPIOOff(GPIO_20);
        }
    }
}

/**
 * @brief Tarea encargada de controlar la humedad de la planta
 */
static void Control_Humedad(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        if (flg_on)
        {
            sensor_humedad = GPIORead(GPIO_23); // si este pin cambia de 0-1 esq se necesita accionar la bomba de agua.

            if (sensor_humedad)
            {
                GPIOOn(GPIO_22); // se acciona la bomba de agua.
            }
        }
        if (flg_off)
        {
            GPIOOff(GPIO_22);
        }
    }
}

/**
 * @brief Tarea encargada de brindar la comunicacion de estados por UART
 */
static void Comunicacion_UART(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */

        if (flg_on)
        {
            if (nivel_ph < 6.0)
            {
                UartSendString(UART_PC, " PH: ");
                UartSendString(UART_PC, UartItoa(nivel_ph, 10));
                UartSendString(UART_PC, " , Acido  ");
                UartSendString(UART_PC, "  Bomba de solucion basica accionada   \r\n");
            }
            if ((nivel_ph <= 6.0) && (nivel_ph >= 6.7))
            {
                UartSendString(UART_PC, " PH: ");
                UartSendString(UART_PC, UartItoa(nivel_ph, 10));
                UartSendString(UART_PC, " , Normal");
            }
            if (nivel_ph > 6.7)
            {
                UartSendString(UART_PC, " PH: ");
                UartSendString(UART_PC, UartItoa(nivel_ph, 10));
                UartSendString(UART_PC, " , Basico  ");
                UartSendString(UART_PC, "  Bomba de solucion acida accionada   \r\n");
            }

            if (sensor_humedad == false)
            {
                UartSendString(UART_PC, "   Humedad Correcta: ");
            }
            else
            {
                UartSendString(UART_PC, "   Humedad Incorrecta: ");
                UartSendString(UART_PC, "   Bomba de agua accionada \r\n ");
            }
        }
    }
}

/** @fn void interrupcion_Tecla1(void)
  @brief Interrupcion asociada a la tecla 1. Cambia su estado  a on ,en el caso de ser presionada.
  @param[in] null
  @retval null
 */
void interrupcion_Tecla1(void)
{
    flg_on = !flg_on;
}

/** @fn void interrupcion_Tecla2(void)
  @brief Interrupcion asociada a la tecla 2. Cambia su estado a off en el caso de ser presionada.
  @param[in] null
  @retval null
 */
void interrupcion_Tecla2(void)
{
    flg_off = !flg_off;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

    /*Interrupcion de teclas*/ // PAra el encendido y apagado

    SwitchesInit();
    SwitchActivInt(SWITCH_1, &interrupcion_Tecla1);
    SwitchActivInt(SWITCH_2, &interrupcion_Tecla2);

    /* Inicialización de timers */
    timer_config_t timer_ph_humedad = {
        .timer = TIMER_A,
        .period = PERIOD_PH_HUMEDAD,
        .func_p = FuncTimerA,
        .param_p = NULL};
    TimerInit(&timer_ph_humedad);

    timer_config_t timer_uart = {
        .timer = TIMER_B,
        .period = PERIOD_UART,
        .func_p = FuncTimerB,
        .param_p = NULL};
    TimerInit(&timer_uart);

 /* Inicializacion de conversor Analogico/digital */
    analog_input_config_t AD = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .sample_frec = NULL,
        .param_p = NULL};

    AnalogInputInit(&AD);


 /* Inicializacion de comunicacion serie UART */
    serial_config_t uart_pc = {
        .port = UART_PC,
        .baud_rate = 115200,
        .func_p = NULL, /*!< Pointer to callback function to call when receiving data (= UART_NO_INT if not requiered)*/
        .param_p = NULL};
    UartInit(&uart_pc);

    /*  Inits para las bombas */

    GPIOInit(GPIO_20, GPIO_OUTPUT); // Para bomoba PhA
    GPIOInit(GPIO_21, GPIO_OUTPUT); // Para bomoba PhB
    GPIOInit(GPIO_22, GPIO_OUTPUT); // Para bomoba Agua

    // GPIO de entrada para el sensor de humedad

    GPIOInit(GPIO_23, GPIO_INPUT); // Para el sensor de humedad

    /* Creación de tareas */
    xTaskCreate(&Control_PH, "Control del PH", 512, NULL, 5, &task_ph);
    xTaskCreate(&Control_Humedad, "Control de Humedad", 512, NULL, 5, &task_humedad);
    xTaskCreate(&Comunicacion_UART, "Comunicacion de estados", 512, NULL, 5, &task_comunicacion);

    /* Inicialización del conteo de timers */
    TimerStart(timer_ph_humedad.timer);
    TimerStart(timer_uart.timer);
}
