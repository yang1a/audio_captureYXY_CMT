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
 * @brief Microphone class for audio acquisition and processing
 */
class Microphone
{
public:
    /**
     * @brief Constructor
     * @param channel: ADC channel number
     * @param bufferSize: Sample buffer size
     */
    Microphone(ADC_HandleTypeDef* hadc, uint32_t channel, uint16_t bufferSize);

    /**
     * @brief Destructor
     */
    ~Microphone();

    /**
     * @brief Initialize microphone
     * @retval None
     */
    void Init(void);

    /**
     * @brief Start sampling (DMA mode)
     * @retval None
     */
    void StartSampling(void);

    /**
     * @brief Stop sampling
     * @retval None
     */
    void StopSampling(void);

    /**
     * @brief Get sample data
     * @param buffer: Pointer to store sample data
     * @retval None
     */
    void GetSamples(uint16_t* buffer);

    /**
     * @brief Get single sample
     * @retval Sample value
     */
    uint16_t GetSample(uint16_t index);

    /**
     * @brief Get buffer size
     * @retval Buffer size
     */
    uint16_t GetBufferSize(void);

    /**
     * @brief Check if sampling is complete
     * @retval true if complete, false otherwise
     */
    bool IsSamplingComplete(void);

    /**
     * @brief Reset sampling complete flag
     * @retval None
     */
    void ResetSamplingFlag(void);

private:
    ADC_HandleTypeDef* m_hadc;       // ADC handle pointer
    uint32_t m_channel;              // ADC channel
    uint16_t* m_buffer;              // Sample buffer
    uint16_t m_bufferSize;           // Buffer size
    volatile bool m_samplingComplete; // Sampling complete flag

    /**
     * @brief Internal callback for conversion complete
     * @retval None
     */
    void ConversionCompleteCallback(void);

    // Friend function for callback
    friend void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
};

#endif // __cplusplus

#endif /* __MICROPHONE_H */
