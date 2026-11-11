#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "i2c.h" 
#include "esp_pm.h"
#include "esp_log.h"
#include "circular_buffer.h"
#include <math.h>

#define SDA_PIN 33
#define SCL_PIN 32

#define buttonPin 18
#define ledPin 12
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t buttonPressedSemaphore = NULL;

#define stepsGoal 12
/**
 * We choose 20ms which gives us 50hz, according to our calculations we would need around 10-15hz
 * but we wanted som margin for errors and therefor chose 20 which gives us 50hz. 
 * Its not really high so it wont consume very much energy
 */
#define SAMPLING_PERIOD 20        //     
#define ALGO_PERIOD 2000            // every 2000ms we check buffer   
#define BUFF_SIZE 256            // around 5 secs of data       
#define MIN_SD 200u              // filter out noise or inactivity        
#define K 2.5                    // multiplier       
#define MIN_INTRA_STEP_TIME 300  // minimum time (ms) between two steps being recognized       
int step_count = 0;


typedef struct {
    int32_t data[BUFF_SIZE];
    int head;   
    int tail;   
    int count;  
} buf_handle_t;

buf_handle_t buffer;



// Initialize buffer
void buffer_init(buf_handle_t *buf) {
    buf->head = 0;
    buf->tail = 0;
    buf->count = 0;
}

// Add value to buffer (overwrite oldest if full)
void buffer_add(buf_handle_t *buf, int32_t val) {
    buf->data[buf->head] = val;
    buf->head = (buf->head + 1) % BUFF_SIZE;
    if (buf->count < BUFF_SIZE) {
        buf->count++;
    } else {
        buf->tail = (buf->tail + 1) % BUFF_SIZE;  
    }
}

// Print buffer content
void buffer_print(buf_handle_t *buf) {
    printf("Buffer: ");
    for (int i = 0; i < buf->count; i++) {
        int idx = (buf->tail + i) % BUFF_SIZE;
        printf("%ld ", buf->data[idx]);
    }
    printf("\n");
}

void button_handler(void *arg) {
    xSemaphoreGiveFromISR(xSemaphore, NULL);
}

void led_task(void *arg) {
    while (1) {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            if (step_count >= stepsGoal) {
                for (int i = 0; i < 4; i++) {
                    gpio_set_level(ledPin, 1);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    gpio_set_level(ledPin, 0);
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
            } else {
                gpio_set_level(ledPin, 1);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    gpio_set_level(ledPin, 0);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    gpio_set_level(ledPin, 1);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    gpio_set_level(ledPin, 0);
                    vTaskDelay(pdMS_TO_TICKS(200));
                  
            }
        }
    }
}

static void sampling_task(void *arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        float ax = readAccelX();
        float ay = readAccelY();
        float az = readAccelZ();

        float mag_f = sqrtf(ax*ax + ay*ay + az*az);
        int32_t mag = (int32_t)(mag_f * 1000);

        buffer_add(&buffer, mag);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SAMPLING_PERIOD));
    }
}

static void algo_task(void *arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        int size = buffer.count;  
        if (size > 0)
        {
            // Compute mean
            int64_t sum = 0;
            for (int i = 0; i < size; i++)
            {
                int idx = (buffer.tail + i) % BUFF_SIZE;
                sum += buffer.data[idx];
            }
            float mean = (float)sum / size;

            // Compute SD
            float variance = 0;
            for (int i = 0; i < size; i++)
            {
                int idx = (buffer.tail + i) % BUFF_SIZE;
                float diff = buffer.data[idx] - mean;
                variance += diff * diff;
            }
            float sd = sqrtf(variance / size);
            if (sd < MIN_SD) sd = MIN_SD;

            // Step detection
            int lastStepTS = -MIN_INTRA_STEP_TIME; // last step timestamp in ms
            for (int i = 0; i < size; i++)
            {
                int idx = (buffer.tail + i) % BUFF_SIZE;
                int32_t sample = buffer.data[idx];

                int sampleTime = i * SAMPLING_PERIOD; // approximate time of this sample

                if (sample > mean + K * sd &&
                    sampleTime - lastStepTS >= MIN_INTRA_STEP_TIME)
                {
                    step_count++;
                    lastStepTS = sampleTime;
                }
            }

            // Optional: clear buffer after processing
            buffer.count = 0;
            buffer.head = 0;
            buffer.tail = 0;

            printf("Steps counted: %d\n", step_count);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(ALGO_PERIOD));
    }
}



void app_main()
{
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 80,
        .light_sleep_enable = true
    };
    esp_err_t ret = esp_pm_configure(&pm_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE("PM", "Failed to configure power management! Error: %d", ret);
    }

    gpio_config_t led = {0};
    led.mode = GPIO_MODE_OUTPUT;
    led.pin_bit_mask = 1ULL << ledPin;
    gpio_config(&led);

    gpio_config_t btn = {0};
    btn.intr_type = GPIO_INTR_NEGEDGE;
    btn.mode = GPIO_MODE_INPUT;
    btn.pin_bit_mask = 1ULL << buttonPin;
    btn.pull_up_en = 1; 
    gpio_config(&btn);

    gpio_set_level(ledPin, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(ledPin, 0);

    
    
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL) {
        ESP_LOGE("MAIN", "Failed to create semaphore");
    }
    gpio_install_isr_service(0);
    gpio_isr_handler_add(buttonPin, button_handler, NULL);
    
    xTaskCreate(led_task, "led", 2048, NULL, 10, NULL);

    initI2C(SDA_PIN, SCL_PIN);
    buffer_init(&buffer);

    xTaskCreate(sampling_task, "sampling", 2048, NULL, 1, NULL);
    xTaskCreate(algo_task, "algo", 2048, NULL, 0, NULL); 
}

