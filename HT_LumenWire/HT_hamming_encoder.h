/**   
 ******************************************************************************   
 * @file    HT_hamming_encoder.h    
 * @author  Andrew Ramanjooloo   
 * @date    3-06-2016
 * @brief   Hamming encoder / decoder
 *
 *		v1.0	-	Hamming (7,4) + even parity check
 *		v2.0	-	Implemented in FreeRTOS with hamenc and hamdec tasks.
 *		v3.0	-	Reverted to 1.0 as tasks & queues not needed.
 *
 ******************************************************************************   
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *	extern uint16_t hamming_byte_encoder(uint8_t input);
 *	extern uint8_t hamming_byte_decoder(uint8_t lower, uint8_t upper);
 *	extern uint16_t get_error_mask(void);
 *	
 ******************************************************************************   
 */

#ifndef HT_hamming_encoder_H
#define HT_hamming_encoder_H


/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/

uint8_t hamming_hbyte_encoder(uint8_t in);
uint8_t hamming_hbyte_decoder(uint8_t in);
extern uint16_t hamming_byte_encoder(uint8_t input);
extern uint8_t hamming_byte_decoder(uint8_t lower, uint8_t upper);
extern uint16_t get_error_mask(void);
#endif
