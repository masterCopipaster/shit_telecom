#ifndef __DEMOD_H
#define __DEMOD_H

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "protocol.h"

#define ADC_UNIT                    ADC_UNIT_1
#define ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define ADC_ATTEN                   ADC_ATTEN_DB_11
#define ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH
#define ADC_MAX_VAL                 ((1 << (ADC_BIT_WIDTH)) - 1)

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define ADC_GET_DATA(p_data)        ((p_data)->type1.data)
#else
#define ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define ADC_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#define ADC_GET_DATA(p_data)        ((p_data)->type2.data)
#endif




#define CONST_PERIOD_2_PI           6.2832 

#define DEMOD_STACK_SIZE            8192

#define DEMOD_SAMPLING_FREQ         20000
#define DEMOD_FREQ_0                FREQ0
#define DEMOD_FREQ_1                FREQ1
#define DEMOD_BITRATE               BITRATE

#define FRAMES_PER_BIT              4

#define SAMPLES_PER_FRAME           (DEMOD_SAMPLING_FREQ / DEMOD_BITRATE / FRAMES_PER_BIT)
#define FRAME_BUFFER_SIZE           ((SOC_ADC_DIGI_RESULT_BYTES * SAMPLES_PER_FRAME) % SOC_ADC_DIGI_DATA_BYTES_PER_CONV == 0 ? (SOC_ADC_DIGI_RESULT_BYTES * SAMPLES_PER_FRAME) : ((SOC_ADC_DIGI_RESULT_BYTES * SAMPLES_PER_FRAME) / SOC_ADC_DIGI_DATA_BYTES_PER_CONV) * (SOC_ADC_DIGI_DATA_BYTES_PER_CONV + 1))
#define DMA_BUFFER_SIZE             (FRAME_BUFFER_SIZE * 2 > 1024 ? FRAME_BUFFER_SIZE * 2 : 1024)
#define BIT_RELATION_TRESH          0.5

void demod_task(void* args);

#endif