
/*==================[inclusions]=============================================*/
#include "ldr.h"
#include <stdint.h>
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "timer_mcu.h"


/*==================[macros and definitions]=================================*/
/*A mayor intensidad de luz mas chica es la resistencia. En oscuridad es mas grande la resistencia*/
#define LDR_DARK 1000	 /*Resistencia en oscuridad, en KΩ. (~1Mohm)*/
#define LDR_10LUX 10	 /*Resistencia a 10 lux, en KΩ. (~1.8 Kohm)*/
#define R_CALIBRACION 47 /*Resistencia calibracion en KΩ*/
#define LUX_NORMAL 1000	 /*lux normal*/

/*==================[internal data declaration]==============================*/
analog_input_config_t LDR_input;

analog_input_config_t LDRinput_1 = {CH0, ADC_SINGLE};
analog_input_config_t LDRinput_2 = {CH1, ADC_SINGLE};
analog_input_config_t LDRinput_3 = {CH2, ADC_SINGLE};
analog_input_config_t LDRinput_4 = {CH3, ADC_SINGLE};

uint16_t ilum_lux_1 = 0;
uint16_t ilum_lux_2 = 0;
uint16_t ilum_lux_3 = 0;
uint16_t ilum_lux_4 = 0;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void LDRs_Init(void)
{
	AnalogInputInit(&LDRinput_1);
	AnalogInputInit(&LDRinput_2);
	AnalogInputInit(&LDRinput_3);
	AnalogInputInit(&LDRinput_4);
};

uint16_t LDRReadLuxIntensity_Top11()
{

	AnalogInputReadSingle(LDRinput_1.input, &ilum_lux_1);
	return ilum_lux_1;
}

uint16_t LDRReadLuxIntensity_Botton12()
{

	AnalogInputReadSingle(LDRinput_2.input, &ilum_lux_2);
	return ilum_lux_2;
}
uint16_t LDRReadLuxIntensity_Right21()
{

	AnalogInputReadSingle(LDRinput_3.input, &ilum_lux_3);
	return ilum_lux_3;
}
uint16_t LDRReadLuxIntensity_Left22()
{

	AnalogInputReadSingle(LDRinput_4.input, &ilum_lux_4);
	return ilum_lux_4;
}

// ilum_lux = (luz_aux_dig * LDR_DARK * 10) / (LDR_10LUX * R_CALIBRACION * (1024 - luz_aux_dig));

int8_t Grados_Vertical_LDR(){

	float aux_arriba=(ilum_lux_1 + ilum_lux_3)/2; 
	float aux_abajo=(ilum_lux_2 + ilum_lux_4)/2; 

	float error_vert= aux_arriba - aux_abajo; 

	int8_t posicion_vertical= abs(error_vert); 

	return posicion_vertical; 

}

int8_t Grados_Horizontal_LDR(){

	float aux_derecha=(ilum_lux_3 + ilum_lux_2)/2; 
	float aux_izq=(ilum_lux_1 + ilum_lux_4)/2; 

	float error_horizontal= aux_derecha - aux_izq; 

	int8_t posicion_horizontal= abs(error_horizontal); 

	return posicion_horizontal; 

}



/*==================[end of file]============================================*/
