#ifndef __MOD_H
#define __MOD_H

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/dac_continuous.h"
#include "esp_check.h"

#include "protocol.h"

#define MOD_SAMPLING_FREQ 20000
#define MOD_BITRATE BITRATE
#define MOD_SAMLES_PER_BIT (MOD_SAMPLING_FREQ/MOD_BITRATE)

#define CONST_PERIOD_2_PI           6.2832 

#define DEMOD_STACK_SIZE            8192

#endif