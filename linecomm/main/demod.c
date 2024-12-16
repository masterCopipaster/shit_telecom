#include "demod.h"
#include <math.h>


#if CONFIG_IDF_TARGET_ESP32
static adc_channel_t channel[1] = {ADC_CHANNEL_6};
#else
static adc_channel_t channel[] = {ADC_CHANNEL_2};
#endif



static TaskHandle_t s_task_handle;
static const char *TAG = "DEMOD";
static uint8_t result[FRAME_BUFFER_SIZE] = {0};
static float data_float[SAMPLES_PER_FRAME];

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = DMA_BUFFER_SIZE,
        .conv_frame_size = FRAME_BUFFER_SIZE,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = DEMOD_SAMPLING_FREQ,
        .conv_mode = ADC_CONV_MODE,
        .format = ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT;
        adc_pattern[i].bit_width = ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}

static float goertzel_mag(int numSamples,float TARGET_FREQUENCY,int SAMPLING_RATE, float* data)
{
    int     k,i;
    float   floatnumSamples;
    float   omega,sine,cosine,coeff,q0,q1,q2,magnitude,real,imag;

    float   scalingFactor = numSamples / 2.0;

    floatnumSamples = (float) numSamples;
    k = (int) (0.5 + ((floatnumSamples * TARGET_FREQUENCY) / (float)SAMPLING_RATE));
    omega = (CONST_PERIOD_2_PI * k) / floatnumSamples;
    sine = sin(omega);
    cosine = cos(omega);
    coeff = 2.0 * cosine;
    q0=0;
    q1=0;
    q2=0;

    for(i=0; i<numSamples; i++)
    {
        q0 = coeff * q1 - q2 + data[i];
        q2 = q1;
        q1 = q0;
    }

    // calculate the real and imaginary results
    // scaling appropriately
    real = (q1 * cosine - q2) / scalingFactor;
    imag = (q1 * sine) / scalingFactor;

    magnitude = sqrtf(real*real + imag*imag);
    //phase = atan(imag/real)
    return magnitude;
}



static void IRAM_ATTR decoder_handler(float mag0, float mag1)
{
    static float runmean_arr[FRAMES_PER_BIT] = {};
    static int runmean_ind = 0;
    static float runmean_sum = 0;

    float maglog = log(mag1/mag0);
    int bit = fabs(maglog) > BIT_RELATION_TRESH ? maglog > 0 ? 1 : -1 : 0;
    
    runmean_sum -= runmean_arr[runmean_ind];
    //runmean_arr[runmean_ind] = maglog;
    runmean_arr[runmean_ind] = (float)bit;
    runmean_sum += runmean_arr[runmean_ind];
    runmean_ind = (runmean_ind + 1) % FRAMES_PER_BIT;

    float mean = runmean_sum / FRAMES_PER_BIT;
    //if (bit >= 0)
    //    ESP_LOGI(TAG, "mag0: %f mag1: %f avg: %f log %f bit %d %d %d", mag0, mag1, avg, maglog, bit, samplen, SAMPLES_PER_FRAME);
    printf("mag0:% 5f,mag1:% 5f,log:% 5f,bit:% 2d,mean:% 5f\n", mag0, mag1, maglog, bit, mean);
}

void IRAM_ATTR demod_task(void* args)
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    float mag0 = 0, mag1 = 0, avg = 0;
    s_task_handle = xTaskGetCurrentTaskHandle();
    memset(result, 0xcc, FRAME_BUFFER_SIZE);
    adc_continuous_handle_t handle = NULL;
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));

    while(1) {

        /**
         * This is to show you the way to use the ADC continuous mode driver event callback.
         * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
         * However in this example, the data processing (print) is slow, so you barely block here.
         *
         * Without using this event callback (to notify this task), you can still just call
         * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        ret = adc_continuous_read(handle, result, FRAME_BUFFER_SIZE, &ret_num, 0);
        if (ret == ESP_OK) {
            //ESP_LOGI("TASK", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
            avg = 0;
            int samplen = 0;
            for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                samplen = i / SOC_ADC_DIGI_RESULT_BYTES;
                adc_digi_output_data_t *p = (void*)&result[i];
                uint32_t data = ADC_GET_DATA(p);
                data_float[samplen] = (float)data / (float)ADC_MAX_VAL;
                avg += data_float[samplen] / SAMPLES_PER_FRAME;
                //printf("%ld\n", data);
                /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                /*
                uint32_t chan_num = ADC_GET_CHANNEL(p);
                if (chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT)) {
                    ESP_LOGI(TAG, "Channel: %"PRIu32", Value: %"PRIx32, chan_num, data);
                } else {
                    ESP_LOGW(TAG, "Invalid data [%"PRIu32"_%"PRIx32"]", chan_num, data);
                }
                */
            }
            //avg = avg / SAMPLES_PER_FRAME;
            for(int i = 0; i < SAMPLES_PER_FRAME; i++)
            data_float[i] = data_float[i] - avg; 
            mag0 = goertzel_mag(SAMPLES_PER_FRAME, DEMOD_FREQ_0, DEMOD_SAMPLING_FREQ, data_float);
            mag1 = goertzel_mag(SAMPLES_PER_FRAME, DEMOD_FREQ_1, DEMOD_SAMPLING_FREQ, data_float);
            decoder_handler(mag0, mag1);
            /**
             * Because printing is slow, so every time you call `ulTaskNotifyTake`, it will immediately return.
             * To avoid a task watchdog timeout, add a delay here. When you replace the way you process the data,
             * usually you don't need this delay (as this task will block for a while).
             */
            //vTaskDelay(1);
        } else if (ret == ESP_ERR_TIMEOUT) {
            //We try to read `FRAME_BUFFER_SIZE` until API returns timeout, which means there's no available data
            break;
        }
    }

    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
}