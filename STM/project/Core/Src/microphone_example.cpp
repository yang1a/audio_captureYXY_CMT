/**
  ******************************************************************************
  * @file           : microphone_example.cpp
  * @brief          : Microphone class usage example
  ******************************************************************************
  */

#ifdef __cplusplus

#include "microphone.h"
#include <iostream>

// ADC handle (declared in main.c or adc.c)
extern ADC_HandleTypeDef hadc1;

// Microphone instances
Microphone* mic0 = nullptr;
Microphone* mic1 = nullptr;
Microphone* mic2 = nullptr;
Microphone* mic3 = nullptr;

#define SAMPLE_BUFFER_SIZE 1024

/**
  * @brief Initialize microphones
  */
void Microphones_Init(void)
{
    // Create microphone instances
    // Note: Adjust channel numbers according to your hardware configuration
    mic0 = new Microphone(&hadc1, ADC_CHANNEL_0, SAMPLE_BUFFER_SIZE);
    mic1 = new Microphone(&hadc1, ADC_CHANNEL_1, SAMPLE_BUFFER_SIZE);
    mic2 = new Microphone(&hadc1, ADC_CHANNEL_2, SAMPLE_BUFFER_SIZE);
    mic3 = new Microphone(&hadc1, ADC_CHANNEL_3, SAMPLE_BUFFER_SIZE);

    // Initialize all microphones
    if (mic0 != nullptr) mic0->Init();
    if (mic1 != nullptr) mic1->Init();
    if (mic2 != nullptr) mic2->Init();
    if (mic3 != nullptr) mic3->Init();
}

/**
  * @brief Start all microphones sampling
  */
void Microphones_Start(void)
{
    if (mic0 != nullptr) mic0->StartSampling();
    if (mic1 != nullptr) mic1->StartSampling();
    if (mic2 != nullptr) mic2->StartSampling();
    if (mic3 != nullptr) mic3->StartSampling();
}

/**
  * @brief Stop all microphones sampling
  */
void Microphones_Stop(void)
{
    if (mic0 != nullptr) mic0->StopSampling();
    if (mic1 != nullptr) mic1->StopSampling();
    if (mic2 != nullptr) mic2->StopSampling();
    if (mic3 != nullptr) mic3->StopSampling();
}

/**
  * @brief Process microphone data
  */
void Microphones_Process(void)
{
    uint16_t buffer[SAMPLE_BUFFER_SIZE];

    // Get samples from microphone 0
    if (mic0 != nullptr && mic0->IsSamplingComplete())
    {
        mic0->GetSamples(buffer);

        // Process samples here...
        // For example: calculate RMS, detect sound events, etc.

        // Reset flag for next sampling
        mic0->ResetSamplingFlag();
    }
}

/**
  * @brief Cleanup microphones
  */
void Microphones_DeInit(void)
{
    if (mic0 != nullptr) delete mic0;
    if (mic1 != nullptr) delete mic1;
    if (mic2 != nullptr) delete mic2;
    if (mic3 != nullptr) delete mic3;

    mic0 = nullptr;
    mic1 = nullptr;
    mic2 = nullptr;
    mic3 = nullptr;
}

#endif // __cplusplus
