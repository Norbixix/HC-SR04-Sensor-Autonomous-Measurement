/*
 * hcsr04.h
 *
 *  Created on: Mar 5, 2026
 *      Author: norbe
 */

#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

typedef enum{
	HCSR04_OK, 		// 1
	HCSR04_ERROR 	// 0
}HCSR04_Status_t;

// HCSR04 structure
typedef struct{
	TIM_HandleTypeDef 	*htim_echo;
	TIM_HandleTypeDef 	*htim_trigger;

	uint32_t			TriggerTimChannel;

	uint32_t			EchoTimChannelStart;
	uint32_t			EchoTimChannelStop;

	// Variable used to store the measured time in microseconds
	uint16_t			Result_us;

}HCSR04_t;

void HCSR04_InterruptHandler(HCSR04_t *hcsr04);

// HCSR04 initialization function
HCSR04_Status_t HCSR04_Init(HCSR04_t *hcsr04, TIM_HandleTypeDef *timer_trigger, uint32_t TriggerTimChannel, TIM_HandleTypeDef *timer_echo, uint32_t EchoTimChannelStart, uint32_t EchoTimChannelStop);

// Function calculating distance in centimeters (floating point)
HCSR04_Status_t HCSR04_GetDistanceFloat(HCSR04_t *hcsr04, float *Result);

#endif /* INC_HCSR04_H_ */
