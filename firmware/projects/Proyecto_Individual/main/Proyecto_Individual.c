/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 and LED_2 blink at different rates, using FreeRTOS tasks and timer interrupts.
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "ldr.h"
#include "servo_sg90.h"
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "ble_mcu.h"

/*==================[macros and definitions]=================================*/
#define PERIOD_LDR 1000000 //(1s)
//#define CONFIG_BLINK_PERIOD_LED_2_US 1300000
#define LUX_NORMAL 500

/*==================[internal data definition]===============================*/
TaskHandle_t ldr_task = NULL;
TaskHandle_t led2_task_handle = NULL;

/*Variables para la comunicacion bluetooth*/
bool flg_on_off = false; //true para activar la app
bool flg_auto_manual = false; // true para activar el modo manual. 
uint8_t mode=0; // Modo manual o automatico. valores ('B'=on) ('b'=off).

//int direction ;

typedef enum {
    UP=1,
    DOWN=3, 
    RIGHT=2,
    LEFT=4
} botones;
botones direction;
 
/* -------Variables LDR-----------*/
/* Inputs para los LDRs, canales para la ADC. */
uint8_t ldr_arriba_input = CH0;  // Norte
uint8_t ldr_abajo_input = CH1;   // Sur
uint8_t ldr_derecha_input = CH2; // oeste
uint8_t ldr_izq_input = CH3;     // este

/* Valores de luxes medidos */
uint16_t valor_ldr_arriba = 0;
uint16_t valor_ldr_abajo = 0;
uint16_t valor_ldr_derecha = 0;
uint16_t valor_ldr_izq = 0;

/*-------Variables de los Servos------*/
/*Variable GPIO para los servos*/
typedef struct
{
    gpio_t pin; /*!< GPIO pin number */
    io_t dir;   /*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerLDR(void *param)
{
    xTaskNotifyGive(ldr_task); /* Envía una notificación a la tarea asociada al LED_1 */
}


/**
 * @brief Tarea encargada de sensar la intendsidad de luz.
 */
static void SensarIntensidadLuz(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        valor_ldr_arriba = LDRReadLuxImtensity();
        valor_ldr_abajo = LDRReadLuxImtensity();
        valor_ldr_derecha = LDRReadLuxImtensity();
        valor_ldr_izq = LDRReadLuxImtensity();
    }
}


void Recepcion_BL(void) // Recepcion de bluetooth. 
{
    uint8_t valor;
     
    UartReadByte(UART_CONNECTOR, &valor);
    switch (valor)
    {
    case 'C': /*Valor para On, app*/
        flg_on_off = !flg_on_off;
        break;
    case 'c': /*Valor para Off, app*/
        flg_on_off = flg_on_off;
        break;

    case 'B': /*Valor on para activar el modo manual*/
        flg_auto_manual = !flg_auto_manual;
        mode= 'B'; 
        break;
    case 'b': /*Valor off para apagar el modo manual*/
        flg_on_off = flg_on_off;
        mode='b'; 
        break;
    
    case '1': /*Valor para mover arriba*/
        direction = UP;
        //up=1;
        break;
    
    case '3': /*Valor para mover abajo*/
        direction = DOWN;
        break;
    
    case '2': /*Valor para mover derecha*/
        direction = RIGHT;
        break;

    case '4': /*Valor para mover izquierda*/
        direction = LEFT;
        break;
    
    default:
        break;
    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
    /* Inicialización de timers */
    timer_config_t timer_ldr = {
        .timer = TIMER_A,
        .period = PERIOD_LDR,
        .func_p = FuncTimerLDR,
        .param_p = NULL};
    TimerInit(&timer_ldr);

    /**Inicializacion de los LDRs*/
    LDR_Init(ldr_arriba_input);
    LDR_Init(ldr_abajo_input);
    LDR_Init(ldr_derecha_input);
    LDR_Init(ldr_izq_input);

    /*Inicializacion de los Servos*/
    gpioConf_t servo1 = {
        .pin = GPIO_18,
        .dir = GPIO_OUTPUT};
    GPIOInit(servo1.pin, servo1.dir);

    gpioConf_t servo2 = {
        .pin = GPIO_19,
        .dir = GPIO_OUTPUT};
    GPIOInit(servo2.pin, servo2.dir);

  /*  serial_config_t Puerto_Serie = {
        .port = UART_CONNECTOR,
        .baud_rate = 9600,
        .func_p = Recepcion_BL,
        .param_p = NULL};
    UartInit(&Puerto_Serie);
*/

 ble_config_t ble_configuration = {
        "ESP_EDU_1",
        read_data
    };

    LedsInit();
    BleInit(&ble_configuration);
    /* Creación de tareas */
    xTaskCreate(&SensarIntensidadLuz, "Sensado de luz", 512, NULL, 5, &ldr_task);
 
    /* Inicialización del conteo de timers */
    TimerStart(timer_ldr.timer);
}
