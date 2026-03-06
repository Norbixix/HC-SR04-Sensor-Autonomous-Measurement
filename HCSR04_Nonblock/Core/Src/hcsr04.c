/*
 * hcsr04.c
 *
 *  Created on: Mar 5, 2026
 *      Author: norbe
 */

#include "main.h"
#include "hcsr04.h"
#include "gpio.h"
#include "tim.h"

// Speed of sound constant (m/s) used to calculate distance based on manufacturer data
#define HCSR04_CALCULATION_CONST  ((float)0.01715)

// Interrupt handler function for the HCSR04 sensor
// Echo time is calculated based on the difference between two captured timestamps
// Echo measurement is performed using two timer channels
void HCSR04_InterruptHandler(HCSR04_t *hcsr04){
	hcsr04->Result_us = __HAL_TIM_GET_COMPARE(hcsr04->htim_echo, hcsr04->EchoTimChannelStop) - __HAL_TIM_GET_COMPARE(hcsr04->htim_echo, hcsr04->EchoTimChannelStart);
}


HCSR04_Status_t HCSR04_Init(HCSR04_t *hcsr04, TIM_HandleTypeDef *timer_trigger, uint32_t TriggerTimChannel, TIM_HandleTypeDef *timer_echo, uint32_t EchoTimChannelStart, uint32_t EchoTimChannelStop){

	// Initialize timer pointers
	hcsr04->htim_trigger = timer_trigger;
	hcsr04->htim_echo = timer_echo;

    // Initialize trigger and echo channels
	hcsr04->TriggerTimChannel = TriggerTimChannel;
	hcsr04->EchoTimChannelStart = EchoTimChannelStart;
	hcsr04->EchoTimChannelStop = EchoTimChannelStop;

	// Start the timer used for echo measurement and configure the echo channel
	HAL_TIM_Base_Start(hcsr04->htim_echo);
	HAL_TIM_IC_Start(hcsr04->htim_echo, hcsr04->EchoTimChannelStart);

	// Input capture in interrupt mode – interrupts are handled by our interrupt handler
	HAL_TIM_IC_Start_IT(hcsr04->htim_echo, hcsr04->EchoTimChannelStop);

	// Start the timer used for trigger generation (it may be the same timer as echo,
	// so this is somewhat redundant but keeps the code universal in case another timer is used),
	// then configure the trigger channel
	// Start PWM generation for the trigger signal
	HAL_TIM_Base_Start(hcsr04->htim_trigger);
	HAL_TIM_PWM_Start(hcsr04->htim_trigger, hcsr04->TriggerTimChannel);

	return HCSR04_OK;
}


// Function calculating distance in centimeters (floating point)
// Distance is calculated based on the echo time (Result_us) and the speed of sound constant
// Result_us contains the measured echo time in microseconds
HCSR04_Status_t HCSR04_CalculateResultFloat(HCSR04_t *hcsr04, float *Result){

	// Result in centimeters
	*Result = (float)hcsr04->Result_us * HCSR04_CALCULATION_CONST;

	return HCSR04_OK;
}


// Function calculating distance in centimeters (integer)
HCSR04_Status_t HCSR04_CalculateResultInteger(HCSR04_t *hcsr04, uint16_t *Result)
{
	// Result in centimeters
	*Result = hcsr04->Result_us / 58; // division by 58 comes from the sensor manufacturer documentation

    // Limit the result to the maximum value of 400 cm if the measurement exceeds this range
	if(*Result > 400){
		*Result = 400;
	}

    // Limit the result to the minimum value of 2 cm if the measurement is below this range
	if(*Result < 2){
		*Result = 2;
	}
	// HCSR04 measurement range according to the manufacturer: 2–400 cm, in real life it works well up to 200cm

	return HCSR04_OK;
}
