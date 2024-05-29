
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
analog_input_config_t LDR_input;

analog_input_config_t LDRinput_1;
analog_input_config_t LDRinput_2;
analog_input_config_t LDRinput_3;
analog_input_config_t LDRinput_4;

uint16_t ilum_lux_1 = 0;
uint16_t ilum_lux_2 = 0;
uint16_t ilum_lux_3 = 0;
uint16_t ilum_lux_4 = 0;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void LDRs_Init(uint8_t Channel)
{
	switch (Channel)
	{
	case CH0:
		LDRinput_1.input = CH0;
		LDRinput_1.mode = ADC_SINGLE;
		LDRinput_1.func_p = NULL;
		AnalogInputInit(&LDRinput_1);
		break;

	case CH1:
		LDRinput_2.input = CH1;
		LDRinput_2.mode = ADC_SINGLE;
		LDRinput_2.func_p = NULL;
		AnalogInputInit(&LDRinput_2);
		break;

	case CH2:
		LDRinput_3.input = CH2;
		LDRinput_3.mode = ADC_SINGLE;
		LDRinput_3.func_p = NULL;
		AnalogInputInit(&LDRinput_3);
		break;

	case CH3:
		LDRinput_4.input = CH3;
		LDRinput_4.mode = ADC_SINGLE;
		LDRinput_4.func_p = NULL;
		AnalogInputInit(&LDRinput_4);
		break;
	
	default:
		break;
	}
}

void LDRReadLuxImtensity(uint8_t Channel, uint16_t *valor)
{
	switch (Channel)
	{
	case CH0:
	AnalogInputReadSingle(LDRinput_1.input, &valor);
		break;

	case CH1:
		AnalogInputReadSingle(LDRinput_2.input, &valor);
		break;

	case CH2:
	AnalogInputReadSingle(LDRinput_3.input, &valor);
		break;

	case CH3:
	AnalogInputReadSingle(LDRinput_4.input, &valor);
		break;
	
	default:
		break;
	}

	// ilum_lux = (luz_aux_dig * LDR_DARK * 10) / (LDR_10LUX * R_CALIBRACION * (1024 - luz_aux_dig));
}




/*==================[end of file]============================================*/
