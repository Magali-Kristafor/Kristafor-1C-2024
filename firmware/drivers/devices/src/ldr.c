
/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <analog_io.h>
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "timer.h"

/*==================[macros and definitions]=================================*/
/*A mayor intensidad de luz mas chica es la resistencia. En oscuridad es mas grande la resistencia*/
#define LDR_DARK 1000	 /*Resistencia en oscuridad, en KΩ. (~1Mohm)*/
#define LDR_10LUX 10	 /*Resistencia a 10 lux, en KΩ. (~1.8 Kohm)*/
#define R_CALIBRACION 47 /*Resistencia calibracion en KΩ*/
#define LUX_NORMAL 1000	 /*lux normal*/

/*==================[internal data declaration]==============================*/
analog_input_config_t LDRinput;
//uint16_t luz_aux_dig;


/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

bool LDR_Init(uint8_t Channel)
{
	LDRinput.input = Channel;
	LDRinput.mode = ADC_SINGLE;
	LDRinput.func_p = NULL;
	LDRinput.param_p = NULL;
	AnalogInputInit(&LDRinput); 
	return true; 
}

uint16_t LDRReadLuxImtensity(void)
{
	uint16_t ilum_lux=0;
	AnalogInputReadSingle(LDRinput.input, &ilum_lux);
	//ilum_lux = (luz_aux_dig * LDR_DARK * 10) / (LDR_10LUX * R_CALIBRACION * (1024 - luz_aux_dig)); 
	return ilum_lux; 
	
}
/*==================[end of file]============================================*/
