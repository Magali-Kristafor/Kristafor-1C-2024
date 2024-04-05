/*! @mainpage Guia1_Ejercicio4
 *
 * @section genDesc General Description
 *
 * Aplicacion que permite mostrar un numero, hasta 3 digitos, por un display LCD. 
 * ( Se realizaron los ejercicios 4, 5 y 6 )
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * 	Display LCD
 * |    Peripheral      |   ESP32   	|
 * |:-----------------: |:--------------|
 * | 	PIN_D1	 	  	| 	GPIO_20		|
 * | 	PIN_D2	 		| 	GPIO_21		|
 * | 	PIN_D3	 		| 	GPIO_22		|
 * | 	PIN_D4	 		| 	GPIO_23		|
 * | 	PIN_SEL_1	 	| 	GPIO_19		|
 * | 	PIN_SEL_2	 	| 	GPIO_18		|
 * | 	PIN_SEL_3	 	| 	GPIO_9		|
 * | 	Vcc= +5V 		| 	Vcc= +5V	|
 * | 	GND 			| 	GND			|
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/03/2024 | Document creation		                         |
 * | 03/04/2024 | Last modification 	                         |
 *
 * @author Magali Kristafor (magali.kristafor@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
/**
 * Estructura para trabajar los puertos GPIO. 
*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/

} gpioConf_t;


/*==================[internal functions declaration]=========================*/

/*Ejercicio 4*/
/*
Escriba una función que reciba un dato de 32 bits,  
la cantidad de dígitos de salida y un puntero a un arreglo 
donde se almacene los n dígitos. La función deberá convertir el dato recibido a BCD, 
guardando cada uno de los dígitos de salida en el arreglo pasado como puntero. 
*/

/** @fn convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
 * @brief Descompone un numero entero en digitos y los almacena en un vector. 
 * @param Dato de 32 bits, cantidad de digitos y un puntero a un arreglo, donde se almacenan los digitos.
 * @retval Null. 
*/

void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	int i=(digits-1);
    while(i>=0){
		bcd_number[i]=data%10;
		data=data/10;
		--i;
	}
	
}

/* Ejercicio 5 */ 
/*
Escribir una función que reciba como parámetro un dígito BCD y 
un vector de estructuras del tipo gpioConf_t. 
 La función deberá cambiar el estado de cada GPIO, a ‘0’ o a ‘1’, 
según el estado del bit correspondiente en el BCD ingresado.
*/

/** @fn void BCDtoGPIO(uint32_t bcd, gpioConf_t *vectPins)
 * @brief Establece el estado de cada GPIO, a '0' o a '1', segun el estado del bit que corresponde en el BCD ingresado. 
 * @param Digito BCD y un vector de estrucura del tipo gpioConf_t. 
 * @retval Null. 
*/

void BCDtoGPIO(uint32_t bcd, gpioConf_t *vectPins) {
	uint8_t mask=1; 

	for(int i=0; i<4; i++){

		if(mask & bcd){
			GPIOOn(vectPins[i].pin); 

		}else{
			GPIOOff(vectPins[i].pin);
		}
		mask=mask<<1;
	}
}


// Ejercicio 6
/*
Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida y 
dos vectores de estructuras del tipo  gpioConf_t. Uno  de estos vectores es igual al definido 
en el punto anterior y 
el otro vector mapea los puertos con el dígito del LCD a donde mostrar un dato

Dígito 1 -> GPIO_19
Dígito 2 -> GPIO_18
Dígito 3 -> GPIO_9
*/

/** @fn void MostrarDisplay(uint32_t dato, uint8_t cantidad_digit, gpioConf_t *vecPins, gpioConf_t *vect_LCD_MUX )
 * @brief Muestra por display el numero ingresado, hasta 3 digitos.  
 * @param Dato de 32 bits, la cantidad de dígitos de salida, un vector del tipo gpioConf_t (pines) y un vector del tipo gpioConf_t(MUX).
 * @retval Null. 
*/

void MostrarDisplay(uint32_t dato, uint8_t cantidad_digit, gpioConf_t *vecPins, gpioConf_t *vect_LCD_MUX ){
	
	uint8_t vect[cantidad_digit];
	convertToBcdArray(dato, cantidad_digit, vect);

	for(int j=0; j<cantidad_digit; j++){
		BCDtoGPIO(vect[j], vecPins);
		GPIOOn(vect_LCD_MUX[j].pin);
		GPIOOff(vect_LCD_MUX[j].pin);
	}

}


/*==================[external functions definition]==========================*/
void app_main(void){
	//printf("Hello world!\n");

	uint32_t dato=123; 
	uint8_t digitos= 3; 
	uint8_t vector[3];

	//convertToBcdArray(dato, digitos, vector);

	gpioConf_t mis_pines[4]={
		{GPIO_20, GPIO_OUTPUT}, 
		{GPIO_21, GPIO_OUTPUT},  
		{GPIO_22, GPIO_OUTPUT}, 
		{GPIO_23,GPIO_OUTPUT} };
	
	for(int i=0; i<4; i++){
		GPIOInit(mis_pines[i].pin, mis_pines[i].dir);
	}

	gpioConf_t LCD[3]={
		{GPIO_19, GPIO_OUTPUT},
		{GPIO_18, GPIO_OUTPUT},
		{GPIO_9, GPIO_OUTPUT}
	};

	for(int i=0; i<3; i++){
		GPIOInit(LCD[i].pin, LCD[i].dir);
	}

	// for(uint32_t i=0; i<digitos; i++){
	// 	printf("%d\n", vector[i]); // (¨%d\n¨,variable que quiero pasarle)
	// }

	MostrarDisplay(dato, digitos, mis_pines, LCD); 

	while (1)
	{
		/* code */
	}
	
}
/*==================[end of file]============================================*/