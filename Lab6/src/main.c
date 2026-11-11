#include <stdio.h>
#include <stdlib.h>
#include <esp_task_wdt.h>
#include "driver/gpio.h"
#include <esp32/rom/ets_sys.h>
#include <esp_timer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "fsm.h"

// Detta används för att undvika “debounce”
#define PUSH_TIME_US 400000

struct travel_need {
    int origin;
    int destination;
};

static volatile uint64_t lastPush = -PUSH_TIME_US;
static volatile int travel_need_counter = 0;
static volatile struct travel_need travel_needs[50];
static QueueHandle_t request_queue;

// ISR för knapptryck
static void IRAM_ATTR handle_push(void *arg) {
    QueueHandle_t queue = (QueueHandle_t)arg;
    uint64_t now = esp_timer_get_time();

    if ((now - lastPush) > PUSH_TIME_US) {
        lastPush = now;

        struct travel_need current = travel_needs[travel_need_counter++];

        if (travel_need_counter >= 50) travel_need_counter = 0;

        request_t req = { 
            .origin = current.origin, 
            .destination = current.destination 
        };

        if (uxQueueSpacesAvailable(queue) > 0) {
            xQueueSendFromISR(queue, &req, NULL);
        }
    }
}


static void init_gpio(void) {
    // LED-pinnar
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN_LEVEL_UP) |
                        (1ULL << LED_PIN_LEVEL_MIDDLE) |
                        (1ULL << LED_PIN_LEVEL_DOWN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Knapp
    gpio_config_t button_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&button_conf);

    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);
}

void app_main(void) {
    esp_err_t res;

     travel_needs[0].origin = 2; travel_needs[0].destination = 1;
    travel_needs[1].origin = 1; travel_needs[1].destination = 2;
    travel_needs[2].origin = 1; travel_needs[2].destination = 2;
    travel_needs[3].origin = 0; travel_needs[3].destination = 2;
    travel_needs[4].origin = 2; travel_needs[4].destination = 1;
    travel_needs[5].origin = 0; travel_needs[5].destination = 2;
    travel_needs[6].origin = 1; travel_needs[6].destination = 2;
    travel_needs[7].origin = 1; travel_needs[7].destination = 0;
    travel_needs[8].origin = 0; travel_needs[8].destination = 1;
    travel_needs[9].origin = 1; travel_needs[9].destination = 0;
    travel_needs[10].origin = 1; travel_needs[10].destination = 2;
    travel_needs[11].origin = 0; travel_needs[11].destination = 1;
    travel_needs[12].origin = 0; travel_needs[12].destination = 2;
    travel_needs[13].origin = 0; travel_needs[13].destination = 1;
    travel_needs[14].origin = 0; travel_needs[14].destination = 2;
    travel_needs[15].origin = 0; travel_needs[15].destination = 1;
    travel_needs[16].origin = 2; travel_needs[16].destination = 1;
    travel_needs[17].origin = 2; travel_needs[17].destination = 1;
    travel_needs[18].origin = 1; travel_needs[18].destination = 0;
    travel_needs[19].origin = 2; travel_needs[19].destination = 1;
    travel_needs[20].origin = 1; travel_needs[20].destination = 0;
    travel_needs[21].origin = 0; travel_needs[21].destination = 1;
    travel_needs[22].origin = 1; travel_needs[22].destination = 2;
    travel_needs[23].origin = 0; travel_needs[23].destination = 2;
    travel_needs[24].origin = 2; travel_needs[24].destination = 1;
    travel_needs[25].origin = 1; travel_needs[25].destination = 0;
    travel_needs[26].origin = 1; travel_needs[26].destination = 2;
    travel_needs[27].origin = 0; travel_needs[27].destination = 2;
    travel_needs[28].origin = 1; travel_needs[28].destination = 0;
    travel_needs[29].origin = 1; travel_needs[29].destination = 2;
    travel_needs[30].origin = 0; travel_needs[30].destination = 1;
    travel_needs[31].origin = 1; travel_needs[31].destination = 2;
    travel_needs[32].origin = 0; travel_needs[32].destination = 2;
    travel_needs[33].origin = 0; travel_needs[33].destination = 2;
    travel_needs[34].origin = 1; travel_needs[34].destination = 2;
    travel_needs[35].origin = 2; travel_needs[35].destination = 1;
    travel_needs[36].origin = 0; travel_needs[36].destination = 2;
    travel_needs[37].origin = 1; travel_needs[37].destination = 0;
    travel_needs[38].origin = 0; travel_needs[38].destination = 2;
    travel_needs[39].origin = 2; travel_needs[39].destination = 1;
    travel_needs[40].origin = 0; travel_needs[40].destination = 1;
    travel_needs[41].origin = 0; travel_needs[41].destination = 1;
    travel_needs[42].origin = 0; travel_needs[42].destination = 1;
    travel_needs[43].origin = 1; travel_needs[43].destination = 0;
    travel_needs[44].origin = 0; travel_needs[44].destination = 2;
    travel_needs[45].origin = 2; travel_needs[45].destination = 1;
    travel_needs[46].origin = 2; travel_needs[46].destination = 1;
    travel_needs[47].origin = 2; travel_needs[47].destination = 1;
    travel_needs[48].origin = 0; travel_needs[48].destination = 2;
    travel_needs[49].origin = 1; travel_needs[49].destination = 0;

    init_gpio();

    request_queue = xQueueCreate(10, sizeof(request_t));
    static fsm_ctx_t fsm;
    fsm_init(&fsm, request_queue);

    res = gpio_install_isr_service(0);
    ESP_ERROR_CHECK(res);
    res = gpio_isr_handler_add(BUTTON_PIN, handle_push, request_queue);
    ESP_ERROR_CHECK(res);


    while (1) {
        fsm_run_step(&fsm);
    }
}
