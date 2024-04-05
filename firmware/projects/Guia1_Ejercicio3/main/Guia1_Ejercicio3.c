/*! @mainpage Ejercicio3_Guia1 
 *
 * @section genDesc General Description
 * Se controla con los modos ON, OFF y TOGGLE el encendido, apagado y titileo de los LEDs. 
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/03/2024 | Document creation		                         |
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
#include "switch.h"
/*==================[macros and definitions]=================================*/

#define ON 1 
#define OFF 2 
#define TOGGLE 3 

#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/


/**
 * Esturctura LEDs, para controlar los mismos.
*/
struct leds
{
      uint8_t mode;       //ON, OFF, TOGGLE
	  uint8_t n_led;      //indica el número de led a controlar
	  uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
	  uint32_t periodo;    //indica el tiempo de cada ciclo

} my_leds; 



/*==================[internal functions declaration]=========================*/

/**
 * @fn controlLEDs( struct leds *my_leds)
 * @brief Funcion que permite controlar los LEDs, segun el modo que se seleccione
  @retval null
 */

void controlLEDs( struct leds *my_leds){
uint16_t delay=0; 

switch (my_leds->mode)
{
	case ON: 
	switch (my_leds->n_led)
	{
	case 1:
	LedOn(LED_1);
		/* code */
		break;
	case 2:
	LedOn(LED_2);
		/* code */
		break;
	case 3:
	LedOn(LED_3);
		/* code */
		break;
	}
	break;
	

	case OFF: 
	switch (my_leds->n_led)
	{
	case 1:
	LedOff(LED_1);
		/* code */
		break;
	case 2:
	LedOff(LED_2);
		/* code */
		break;
	case 3:
	LedOff(LED_3);
		/* code */
		break;

	}
	break;
	

case TOGGLE: 
for(int i=0; i<my_leds->n_ciclos; i++){
	switch (my_leds->n_led)
	{
	case 1:
	LedToggle(LED_1);
		/* code */
		break;
	case 2:
	LedToggle(LED_2);
		/* code */
		break;
	case 3:
	LedToggle(LED_3);
		/* code */
		break;
	
	default:
	break;
	}
	

	delay=my_leds->periodo/CONFIG_BLINK_PERIOD;
	for(uint32_t j=0; j<delay; j++){
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
	
}
break;

default:
break;
}

}



/*==================[external functions definition]==========================*/
void app_main(void){

LedsInit();
struct leds my_leds;
my_leds.mode=TOGGLE; 
my_leds.n_led=1; 
my_leds.n_ciclos=10; 
my_leds.periodo=500; 

controlLEDs(&my_leds);

while (1)
{
	/* code */
}

}


/*==================[end of file]============================================*/


