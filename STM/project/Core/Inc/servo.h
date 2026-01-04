// Servo control using TIM1 CH1 (PA8) PWM output
#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32h7xx_hal_tim.h"

// TIM1 handler used for the servo PWM output
extern TIM_HandleTypeDef htim1;

// Initialize TIM1 for 50 Hz PWM on CH1 (PA8) and start output
void MX_TIM1_Init(void);

// Update pulse width in microseconds (typical 500-2500 us)
HAL_StatusTypeDef Servo_SetPulse(uint16_t pulse_us);

// Convenience: map 0-180 degrees to pulse range
HAL_StatusTypeDef Servo_SetAngle(uint8_t angle_deg);

#ifdef __cplusplus
}
#endif

#endif // __SERVO_H__
