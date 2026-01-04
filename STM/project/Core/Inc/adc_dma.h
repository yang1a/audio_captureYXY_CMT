#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_tim.h"

    extern ADC_HandleTypeDef hadc1;
    extern DMA_HandleTypeDef hdma_adc1;
    extern TIM_HandleTypeDef htim2;

    void MX_ADC1_Init(void);
    HAL_StatusTypeDef ADC_DMA_Start(uint16_t *buffer, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
