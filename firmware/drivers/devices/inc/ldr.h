#ifndef LDR_H
#define LDR_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup LDR
 ** @{ */

/** \brief LDR for the ESP-EDU Board.
 *
 * @note This driver can handle up to 4 SG90 microservos.
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/01/2024 | Document creation		                         						|
 * 
 **/

/** Descripcion General.
 * Se utiliza conectada en serie con una resistencia de calibracion de 47Kohm, con una alimentacion de 5V.
 * Este driver inicializa el ADC,y convierte en valor digital de salida del ADC en LUX.
 */

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "analog_io_mcu.h"

/*==================[macros]=================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @fn LDR_Init(uint8_t Chanel)
 * @brief Inicializa el LDR.
 * @param[in] recibe como parametro el canal para el ADC, el cual es de tipo uint8_t.
 */
void LDRs_Init(void);


/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
uint16_t LDRReadLuxIntensity_Top11();

/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
uint16_t LDRReadLuxIntensity_Botton12();
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */

/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
uint16_t LDRReadLuxIntensity_Right21();

/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
uint16_t LDRReadLuxIntensity_Left22();


/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
int8_t Grados_Vertical_LDR();



/**
 * @fn  LDRReadLuxImtensity(void);
 * @brief Lee la intensidad de la luz que sensa.
 * @param[in] 
 * @param[out] Devuelve el valor de intensidad medido 
 */
int8_t Grados_Horizontal_LDR();


#endif /* #ifndef SERVO_SG90_H */

/*==================[end of file]============================================*/

