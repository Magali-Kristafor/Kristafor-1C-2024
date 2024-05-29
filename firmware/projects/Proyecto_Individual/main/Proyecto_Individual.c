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
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "ldr.h"
#include "servo_sg90.h"
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "ble_mcu.h"
#include "servo_sg90.h"
#include "led.h"

/*==================[macros and definitions]=================================*/
#define PERIOD_LDR 1000000 //(1s)
#define CONFIG_BLINK_PERIOD 500
#define LUX_NORMAL 500

#define LED_BT LED_1
/*==================[internal data definition]===============================*/
TaskHandle_t ldr_task = NULL;
TaskHandle_t com_task = NULL;
TaskHandle_t panel_task = NULL;

/*Variables para la comunicacion bluetooth*/
bool flg_on_off = false;      // true para activar la app
bool flg_auto_manual = false; // true para activar el modo manual.
uint8_t mode = 0;             // Modo manual o automatico. valores ('B'=on) ('b'=off).

int direction;

enum
{
    UP = 1,
    DOWN = 3,
    RIGHT = 2,
    LEFT = 4
};

/* -------Variables LDR-----------*/
/* Inputs para los LDRs, canales para la ADC. */

/*
 */
uint8_t ldr_arriba_input = CH0;  // Norte
uint8_t ldr_abajo_input = CH1;   // Sur
uint8_t ldr_derecha_input = CH2; // oeste
uint8_t ldr_izq_input = CH3;     // este

/* Valores de luxes medidos */
uint16_t valor_ldr_arriba = 0;
uint16_t valor_ldr_abajo = 0;
uint16_t valor_ldr_derecha = 0;
uint16_t valor_ldr_izq = 0;


/*------ Variables Servos-----*/

int8_t posicion_vertical=0; 
int8_t posicion_horizontal=0;

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
        valor_ldr_arriba=LDRReadLuxIntensity_Top11();
        valor_ldr_abajo=LDRReadLuxIntensity_Botton12();
        valor_ldr_derecha=LDRReadLuxIntensity_Right21();
        valor_ldr_izq=LDRReadLuxIntensity_Left22();

        //printf("%d,%d,%d,%d\r\n",valor_ldr_arriba,valor_ldr_abajo,valor_ldr_derecha,valor_ldr_izq);
    }
}




/*Funcion para la comunicacion del bl, interrupcion*/
void Recepcion_BL(uint8_t *valor, uint8_t length) // Recepcion de bluetooth.
{
    switch (valor[0])
    {
    case 'C': /*Valor para On, app*/
        flg_on_off = !flg_on_off;
        break;
    case 'c': /*Valor para Off, app*/
        flg_on_off = flg_on_off;
        break;

    case 'B': /*Valor on para activar el modo manual*/
        flg_auto_manual = !flg_auto_manual;
        mode = 'B';
        break;
    case 'b': /*Valor off para apagar el modo manual*/
        flg_on_off = flg_on_off;
        mode = 'b';
        break;

    case '1': /*Valor para mover arriba*/
        direction = UP;
        // up=1;
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

static void Comunicacion_Bl(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        if (BleStatus() == BLE_CONNECTED)
        {
            BleSendString('La intensidad de luz es: '); // Envia info de lo que mide
        }
    }
}


static void Mover_Panel(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        if(flg_auto_manual){
            
        }


    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

    LedsInit();

    /* Inicialización de timers */
    timer_config_t timer_ldr = {
        .timer = TIMER_A,
        .period = PERIOD_LDR,
        .func_p = FuncTimerLDR,
        .param_p = NULL};
    TimerInit(&timer_ldr);

    /**Inicializacion de los LDRs*/
    LDRs_Init();

    /*Inicializacion de los Servos*/
    ServoInit(SERVO_0, GPIO_18);
    ServoInit(SERVO_1, GPIO_19);

    ble_config_t ble_configuration = {
        "ESP_EDU_1",
        Recepcion_BL};
    BleInit(&ble_configuration);
    


    /* Creación de tareas */
    xTaskCreate(&SensarIntensidadLuz, "Sensado de luz", 1024, NULL, 5, &ldr_task);
    xTaskCreate(&Mover_Panel, "Movimiento del panel", 512, NULL, 5, &panel_task);
    //xTaskCreate(&Comunicacion_Bl, "Comunicacion_Bl", 512, NULL, 5, &com_task);
    

    /* Inicialización del conteo de timers */
    TimerStart(timer_ldr.timer);



    while (1)
    {
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        printf("%d,%d,%d,%d\r\n",valor_ldr_arriba,valor_ldr_abajo,valor_ldr_derecha,valor_ldr_izq);
        switch (BleStatus())
        {
        case BLE_OFF:
            LedOff(LED_BT);
            break;
        case BLE_DISCONNECTED:
            LedToggle(LED_BT);
            break;
        case BLE_CONNECTED:
            LedOn(LED_BT);
            break;
        }
    }
}
