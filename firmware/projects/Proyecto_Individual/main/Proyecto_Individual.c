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
#include "math.h"

/*==================[macros and definitions]=================================*/
#define PERIOD_LDR 1000000 //(1s)
#define CONFIG_BLINK_PERIOD 100
#define LUX_NORMAL 500

#define LED_BT LED_1
/*==================[internal data definition]===============================*/
TaskHandle_t ldr_task = NULL;
TaskHandle_t com_task = NULL;
TaskHandle_t panel_task = NULL;

/*Variables para la comunicacion bluetooth*/
bool flg_on_off = false;      // true para activar la app
bool flg_auto_manual = false; // true para activar modo automatico.
uint8_t mode = 0;             // Modo manual o automatico. valores ('B'=on) ('b'=off).

int direction = 0;

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

/* Valores para control auto*/

float aux_abajo;
float aux_arriba;
float aux_derecha;
float aux_izq;

float error_vert;
float error_horizontal;

/*------ Variables Servos-----*/

int8_t posicion_vertical = 1;
int8_t posicion_horizontal = 1;

int8_t grados_1 = 1;
int8_t grados_2 = 1;

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
        valor_ldr_arriba = LDRReadLuxIntensity_Top11();
        valor_ldr_abajo = LDRReadLuxIntensity_Botton12();
        valor_ldr_derecha = LDRReadLuxIntensity_Right21();
        valor_ldr_izq = LDRReadLuxIntensity_Left22();

        // printf("%d,%d,%d,%d\r\n",valor_ldr_arriba,valor_ldr_abajo,valor_ldr_derecha,valor_ldr_izq);
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

    case 'B': /*ON modo automatico*/
        flg_auto_manual = !flg_auto_manual;
        mode = 'B';
        break;
    case 'b': /*Valor off para apagar el modo automatico*/
        flg_on_off = flg_on_off;
        mode = 'b';
        break;

    case '1': /*Valor para mover arriba*/
        direction = UP;
        grados_1 += 5; // up=1;
        break;

    case '3': /*Valor para mover abajo*/
        direction = DOWN;
        grados_1 -= 5;
        break;

    case '2': /*Valor para mover derecha*/
        direction = RIGHT;
        grados_2 += 5;
        break;

    case '4': /*Valor para mover izquierda*/
        direction = LEFT;
        grados_2 -= 5;
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
            BleSendString("La intensidad de luz es: "); // Envia info de lo que mide
        }
    }
}

static void Mover_Panel(void *pvParameter)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // La tarea espera en este punto hasta recibir una notificación

        if (flg_on_off)
        {
            if (flg_auto_manual) // True, modo automatico en on 
            {
                //para los angulos verticales
                aux_abajo = (valor_ldr_abajo + valor_ldr_izq) / 2;
                aux_arriba = (valor_ldr_arriba + valor_ldr_derecha) / 2;
                error_vert = aux_arriba - aux_abajo;
                int8_t angle_vert=Grados_Vertical_LDR();


               // para los angulos horizontales
                aux_derecha = (valor_ldr_derecha + valor_ldr_abajo) / 2;
                aux_izq = (valor_ldr_izq + valor_ldr_arriba) / 2;
                error_horizontal = aux_derecha - aux_izq;
                int8_t angle_hori=Grados_Horizontal_LDR();

                if(error_vert>0){
                    ServoMove(SERVO_0,angle_vert);

                }else{
                    ServoMove(SERVO_0,angle_vert);
                }

                if(error_horizontal>0){
                    ServoMove(SERVO_1,angle_hori);

                }else{
                    ServoMove(SERVO_1,angle_hori);
                }


            }
            else { 
            
            //modo manual
               if (direction==UP || direction==DOWN){
                ServoMove(SERVO_0, grados_1); 
               }
               if(direction==RIGHT || direction==LEFT){
                ServoMove(SERVO_1, grados_2); 
               }
               
            }
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
    ServoInit(SERVO_0, GPIO_18); // servo mueve horizontal
    ServoInit(SERVO_1, GPIO_21); // servo mueve vertical

    ble_config_t ble_configuration = {
        "SOLAR_MAGA",
        Recepcion_BL};
    BleInit(&ble_configuration);

    /* Creación de tareas */
    xTaskCreate(&SensarIntensidadLuz, "Sensado de luz", 1024, NULL, 5, &ldr_task); // listo
    xTaskCreate(&Mover_Panel, "Movimiento del panel", 512, NULL, 5, &panel_task);
    xTaskCreate(&Comunicacion_Bl, "Comunicacion_Bl", 512, NULL, 5, &com_task);

    /* Inicialización del conteo de timers */
    TimerStart(timer_ldr.timer);

    while (1)
    {
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        printf("%d,%d,%d,%.0f\r\n", valor_ldr_arriba, valor_ldr_abajo, valor_ldr_derecha, 2.25*valor_ldr_izq);
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

//para los angulos verticales
                aux_abajo = (valor_ldr_abajo + (valor_ldr_izq*2.25)) / 2;
                aux_arriba = (valor_ldr_arriba + valor_ldr_derecha) / 2;
                error_vert = aux_arriba - aux_abajo;
                //int8_t angle_vert=Grados_Vertical_LDR();


               // para los angulos horizontales
                aux_derecha = (valor_ldr_derecha + valor_ldr_abajo) / 2;
                aux_izq = ((valor_ldr_izq*2.25) + valor_ldr_arriba) / 2;
                error_horizontal = aux_derecha - aux_izq;
                //int8_t angle_hori=Grados_Horizontal_LDR();

                if(error_vert>100){
                    grados_1+=1; 
                    printf("%f\n", error_vert);
                    ServoMove(SERVO_1,grados_1); //mueve para arriba 
                    printf(" Error: %.0f Mueve para arriba \n", error_vert); 

                }
                if(error_vert<-100){
                    grados_1-=1; 
                    ServoMove(SERVO_1,grados_1); //mueve4 para abajo 
                    printf(" Error: %.0f Mueve para abajo \n", error_vert); 
                }

                if(error_horizontal>100){
                    grados_2+=1; 
                    printf("%f\n", error_horizontal);  
                    ServoMove(SERVO_0,grados_2); //mueve para arriba 
                    printf(" Error: %.0f Mueve a la derecha \n", error_horizontal);  
                }
                if(error_horizontal<-100){
                    grados_2-=1; 
                    ServoMove(SERVO_0,grados_2); //mueve4 para abajo 
                    printf(" Error: %.0f Mueve a la izq \n", error_horizontal);
                }
    

    }
}
