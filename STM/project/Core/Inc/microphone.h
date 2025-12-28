#ifndef __MICROPHONE_H
#define __MICROPHONE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "adc.h"


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus


#include <stdint.h>
#include <stdbool.h>

/**
 * @brief MAX9814 Gain Selection
 */
typedef enum
{
    MAX9814_GAIN_40DB  = 0,   // Low level on GAIN pin
    MAX9814_GAIN_50DB  = 1,   // Floating GAIN pin
    MAX9814_GAIN_60DB  = 2    // High level on GAIN pin
} MAX9814_Gain_t;

/**
 * @brief MAX9814 Attack/Release Time
 */
typedef enum
{
    MAX9814_AR_FAST   = 0,    // Low level on A/R pin
    MAX9814_AR_MEDIUM = 1,    // Floating A/R pin
    MAX9814_AR_SLOW   = 2     // High level on A/R pin
} MAX9814_AR_t;

/**
 * @brief Microphone class for MAX9814 audio acquisition and processing
 */
class Mic
{
private:
    int ADC_Cvalue;
    double voltage;

    ADC_HandleTypeDef* hadc; 
    uint32_t channel; 
    uint16_t bufferSize;
    GPIO_TypeDef* GPIOPort; 
    uint16_t GPIOPin;

public:
    /**
     * @brief Constructor
     * @param hadc: ADC handle pointer
     * @param channel: ADC channel number
     * @param bufferSize: Sample buffer size
     * @param GPIOPort: GPIO port (optional, set nullptr if not used)
     * @param GPIOPin: GPIO pin number (optional, set 0 if not used)
     */
    Mic(ADC_HandleTypeDef* hadc, 
               uint32_t channel, 
               uint16_t bufferSize,
               GPIO_TypeDef* GPIOPort = nullptr, 
               uint16_t GPIOPin = 0);

    void init();

    void update();
    double getVoltage();
};

#endif // __cplusplus

#endif /* __MICROPHONE_H */
