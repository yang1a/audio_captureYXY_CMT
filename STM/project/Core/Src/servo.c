/**
 ******************************************************************************
 * @file           : servo.c
 * @brief          : Servo motor PWM control implementation
 ******************************************************************************
 */

#include "servo.h"
#include "utils.h"

/* Private variables */
static TIM_HandleTypeDef *g_htim = NULL;
static uint32_t g_channel = 0;
static uint32_t g_timer_period = 0;

/**
 * @brief Initialize servo control
 */
int Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    if (htim == NULL)
        return -1;

    g_htim = htim;
    g_channel = channel;

    // Calculate timer period for 50Hz (20ms)
    // Assuming timer is configured with appropriate prescaler
    // Period = Timer_Clock / (Prescaler + 1) / Frequency
    // For 50Hz: Period should give 20ms total
    g_timer_period = __HAL_TIM_GET_AUTORELOAD(htim);

    return 0;
}

/**
 * @brief Set servo angle
 * @param angle_deg: Desired angle in degrees (0 to 180)
 */
int Servo_SetAngle(float angle_deg)
{
    if (g_htim == NULL)
        return -1;

    // Clamp angle to valid range
    angle_deg = clamp_f(angle_deg, 0.0f, 180.0f);

    // Map angle to pulse width
    // pulse_ms = SERVO_MIN_MS + (angle / 180) * (SERVO_MAX_MS - SERVO_MIN_MS)
    float pulse_ms = SERVO_MIN_MS + (angle_deg / 180.0f) * (SERVO_MAX_MS - SERVO_MIN_MS);

    // Convert pulse width (ms) to timer compare value
    // duty_cycle = pulse_ms / SERVO_PWM_PERIOD_MS
    float duty_cycle = pulse_ms / SERVO_PWM_PERIOD_MS;
    uint32_t compare_value = (uint32_t)(duty_cycle * (float)g_timer_period);

    // Set PWM compare value
    __HAL_TIM_SET_COMPARE(g_htim, g_channel, compare_value);

    return 0;
}

/**
 * @brief Start servo PWM output
 */
int Servo_Start(void)
{
    if (g_htim == NULL)
        return -1;

    // Set to center position initially
    Servo_SetAngle(SERVO_CENTER_DEG);

    // Start PWM
    return (HAL_TIM_PWM_Start(g_htim, g_channel) == HAL_OK) ? 0 : -1;
}

/**
 * @brief Stop servo PWM output
 */
int Servo_Stop(void)
{
    if (g_htim == NULL)
        return -1;

    return (HAL_TIM_PWM_Stop(g_htim, g_channel) == HAL_OK) ? 0 : -1;
}
