/*! @mainpage Guia1_Ejercicio2
 *
 * \section genDesc General Description
 *
 * Se hace titilar los leds 1 y 2 al mantener presionada las teclas 1 y 2. 
 * A su vez al precionar la tecla 1y2 simultaneamente, se hace titilar el led 3. 
 * 

 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
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
#define CONFIG_BLINK_PERIOD 1000
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
    while(1) {
    	teclas  = SwitchesRead();
    	switch(teclas) {
    		case SWITCH_1:
    			LedToggle(LED_1);
    		break;
    		
			case SWITCH_2:
    			LedToggle(LED_2);
    		break;
			
			case SWITCH_1 | SWITCH_2 :
			LedToggle(LED_3);
			break; 
		}
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
