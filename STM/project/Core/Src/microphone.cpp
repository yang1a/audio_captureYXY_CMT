/**
  ******************************************************************************
  * @file           : microphone.cpp
  * @brief          : Microphone class implementation
  ******************************************************************************
  */

#ifdef __cplusplus

#include "microphone.h"

/**
  * @brief Constructor
  */
Microphone::Microphone(ADC_HandleTypeDef* hadc, uint32_t channel, uint16_t bufferSize)
    : m_hadc(hadc)
    , m_channel(channel)
    , m_buffer(nullptr)
    , m_bufferSize(bufferSize)
    , m_samplingComplete(false)
{
    // Allocate buffer for samples
    m_buffer = new uint16_t[bufferSize];
}

/**
  * @brief Destructor
  */
Microphone::~Microphone()
{
    if (m_buffer != nullptr)
    {
        delete[] m_buffer;
        m_buffer = nullptr;
    }
}

/**
  * @brief Initialize microphone
  */
void Microphone::Init(void)
{
    if (m_hadc == nullptr || m_buffer == nullptr)
    {
        return;
    }

    // Configure ADC channel
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = m_channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(m_hadc, &sConfig) != HAL_OK)
    {
        // Error_Handler();
    }
}

/**
  * @brief Start sampling (DMA mode)
  */
void Microphone::StartSampling(void)
{
    if (m_hadc == nullptr || m_buffer == nullptr)
    {
        return;
    }

    // Clear the buffer
    for (uint16_t i = 0; i < m_bufferSize; i++)
    {
        m_buffer[i] = 0;
    }

    // Reset sampling flag
    m_samplingComplete = false;

    // Start ADC with DMA
    HAL_ADC_Start_DMA(m_hadc, (uint32_t*)m_buffer, m_bufferSize);
}

/**
  * @brief Stop sampling
  */
void Microphone::StopSampling(void)
{
    if (m_hadc == nullptr)
    {
        return;
    }

    HAL_ADC_Stop_DMA(m_hadc);
}

/**
  * @brief Get sample data
  */
void Microphone::GetSamples(uint16_t* buffer)
{
    if (m_buffer == nullptr || buffer == nullptr)
    {
        return;
    }

    // Copy samples to output buffer
    for (uint16_t i = 0; i < m_bufferSize; i++)
    {
        buffer[i] = m_buffer[i];
    }
}

/**
  * @brief Get single sample
  */
uint16_t Microphone::GetSample(uint16_t index)
{
    if (m_buffer == nullptr || index >= m_bufferSize)
    {
        return 0;
    }

    return m_buffer[index];
}

/**
  * @brief Get buffer size
  */
uint16_t Microphone::GetBufferSize(void)
{
    return m_bufferSize;
}

/**
  * @brief Check if sampling is complete
  */
bool Microphone::IsSamplingComplete(void)
{
    return m_samplingComplete;
}

/**
  * @brief Reset sampling complete flag
  */
void Microphone::ResetSamplingFlag(void)
{
    m_samplingComplete = false;
}

/**
  * @brief Internal callback for conversion complete
  */
void Microphone::ConversionCompleteCallback(void)
{
    m_samplingComplete = true;
}

/**
  * @brief HAL ADC Conversion Complete Callback
  */
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Find microphone instance with matching ADC handle and set flag
    // This is a simple implementation - you may need to manage multiple instances
}

#endif // __cplusplus
