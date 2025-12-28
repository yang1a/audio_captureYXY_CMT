/**
  ******************************************************************************
  * @file           : microphone.cpp
  * @brief          : Microphone class implementation for MAX9814
  ******************************************************************************
  */

#include "stm32h7xx_hal.h"
#ifdef __cplusplus

#include "microphone.h"

/**
  * @brief Constructor
  */
Mic::Mic(ADC_HandleTypeDef* hadc, 
          uint32_t channel, 
          uint16_t bufferSize,
          GPIO_TypeDef* GPIOPort, 
          uint16_t GPIOPin)
    : hadc(hadc),
      channel(channel),
      bufferSize(bufferSize),
      GPIOPort(GPIOPort),
      GPIOPin(GPIOPin)
{

}

void Mic::init()
{
  if (GPIOPort != nullptr && GPIOPin != 0)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {};
    GPIO_InitStruct.Pin = GPIOPin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOPort, &GPIO_InitStruct);
  } 

  if (hadc != nullptr)
  {
    ADC_ChannelConfTypeDef sConfig = {};
    
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetRightShift = DISABLE;
    sConfig.OffsetSignedSaturation = DISABLE; 

    // 应用 ADC 通道配置
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
    {
        return;
    }

    // 校准 ADC
    if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        return;
    }
  }
}

#endif // __cplusplus
