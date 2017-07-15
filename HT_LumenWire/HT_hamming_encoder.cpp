/**   
 ******************************************************************************   
 * @file    HT_hamming_encoder.c    
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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "HT_hamming_encoder.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint16_t ErrMask = 0;

/**
  * Implement Hamming Code + parity checking
  * Hamming code is based on the following generator matrix (+ even parity check)
  * G = [ 1 1 1 | 1 0 0 0 ;
  *       1 1 0 | 0 1 0 0 ;
  *       1 0 1 | 0 0 1 0 ;
  *       0 1 1 | 0 0 0 1 ];
  *
  * y = [p0 | x * G]
  */
uint8_t hamming_hbyte_encoder(uint8_t in) {

	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits, assuming MSB ordering */
	d0 = !!(in & 0x1);
	d1 = !!(in & 0x2);
	d2 = !!(in & 0x4);
	d3 = !!(in & 0x8);

	/* calculate hamming parity bits */
	h0 = d0 ^ d1 ^ d2;
	h1 = d0 ^ d1 ^ d3;
	h2 = d0 ^ d2 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 6) | (h1 << 5) | (h2 << 4) |
		(d0 << 3) | (d1 << 2) | (d2 << 1) | (d3 << 0);

	/* calculate even parity bit */
	for (z = 0; z < 7; z++)		
		p0 = p0 ^ !!(out & (1 << z));

	out |= (p0 << 7);

	return(out);
}


/**
  * Implement Hamming Code on a full byte of input
  * This means that 16-bits out output is needed
  */
extern uint16_t hamming_byte_encoder(uint8_t input){

	uint16_t out;

	/* first encode D0..D3 (first 4 bits), 
	 * then D4..D7 (second 4 bits), reconstruct
	 * with LSB ordering, i.e. P0...D3,P1..D7
	 */
	out = (hamming_hbyte_encoder(input & 0xF) << 8)| 
		hamming_hbyte_encoder(input >> 4);

	return(out);
}


/**
  * Implement Hamming parity checking
  * Hamming code is based on the following parity check matrix 
  *	H = [ 1 0 0 | 1 1 1 0 ;
  *   	  0 1 0 | 1 1 0 1 ;
  *       0 0 1 | 1 0 1 1 ];
  *
  * syn = H * y[1:7]'
  */
uint8_t hamming_hbyte_decoder(uint8_t in){
	
	uint8_t s0, s1, s2;
	uint8_t p0, h0, h1, h2, d0, d1, d2, d3;
	uint8_t col_err, p_check = 0, z, out;

	/* extract bits, assuming LSB ordering*/
	p0 = !!(in & 0x80);
	h0 = !!(in & 0x40);
	h1 = !!(in & 0x20);
	h2 = !!(in & 0x10);
	d0 = !!(in & 0x08);
	d1 = !!(in & 0x04);
	d2 = !!(in & 0x02);
	d3 = !!(in & 0x01);

	/* calculate syndrome bits */
	s0 = h0 ^ d0 ^ d1 ^ d2;
	s1 = h1 ^ d0 ^ d1 ^ d3;
	s2 = h2 ^ d0 ^ d2 ^ d3;

	/* Find and correct error */
	if(s0 || s1 || s2){							// at least 1 bit error if true
		if(s0 && !s1 && !s2) col_err = 6;		// error in h0
		else if(!s0 && s1 && !s2) col_err = 5;	// error in h1
		else if(!s0 && !s1 && s2) col_err = 4;	// error in h2
		else if(s0 && s1 && s2) col_err = 3;	// error in d0
		else if(s0 && s1 && !s2) col_err = 2;	// error in d1
		else if(s0 && !s1 && s2) col_err = 1;	// error in d2
		else if(!s0 && s1 && s2) col_err = 0;	// error in d3
		out = in ^ (1 << (col_err));			// flip only the column with the error

		/* calculate even parity bit */
		for (z = 0; z < 7; z++)		
			p_check = p_check ^ !!(out & (1 << z));

		/* check for parity match */
		if(p_check != p0){
			// more than 1 bit error, can't correct
		}	// else 1 bit error is corrected

	} else {
		/* calculate even parity bit */
		for (z = 0; z < 7; z++)		

			p_check = p_check ^ !!(in & (1 << z));

		/* check for parity match */
		if(p_check != p0){
			out = in ^ (1 << 7);			// 1 bit error is the parity, flip it
		} else {
			out = in;						// No errors
		}
	}

	return(out);
}


/**
  * Implement Hamming correction on 2 bytes of input
  * This means that 16-bit input is output as 8-bits
  *
  */
extern uint8_t hamming_byte_decoder(uint8_t lower, uint8_t upper){

	uint8_t out = 0, corrected_lower, corrected_upper;
	uint8_t tmp;
	int z;

	/* first decode D0..D7 (first 8 bits), then D8..D15
	 * (second 8 bits).
	 */
	corrected_upper = hamming_hbyte_decoder(upper);
	corrected_lower = hamming_hbyte_decoder(lower);

	/* Calculate Error Mask */
	ErrMask = ((lower << 8) | upper) ^
		((corrected_lower << 8) | corrected_upper);

	/* Reconstruct in MSB ordering. */
	tmp = ((0xF & corrected_lower) << 4) | (0xF & corrected_upper);
	for(z = 7; z >= 0; z--){
		out |= (!!(tmp & (1 << z)) << (7-z));
	}

	return(out);
}


extern uint16_t get_error_mask(void){
	return ErrMask;
}
