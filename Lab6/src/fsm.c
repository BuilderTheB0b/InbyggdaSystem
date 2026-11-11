#include "fsm.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "HISS";


void fsm_set_leds_for_level(level_t level) {
    gpio_set_level(LED_PIN_LEVEL_UP,     level == LEVEL_UP);
    gpio_set_level(LED_PIN_LEVEL_MIDDLE, level == LEVEL_MIDDLE);
    gpio_set_level(LED_PIN_LEVEL_DOWN,   level == LEVEL_DOWN);
}

void fsm_all_leds_off(void) {
    gpio_set_level(LED_PIN_LEVEL_UP, 0);
    gpio_set_level(LED_PIN_LEVEL_MIDDLE, 0);
    gpio_set_level(LED_PIN_LEVEL_DOWN, 0);
}

void fsm_delay_seconds(uint32_t sec) {
    vTaskDelay(pdMS_TO_TICKS(sec * 1000U));
}

static void st_idle(fsm_ctx_t *ctx);
static void st_moving_up(fsm_ctx_t *ctx);
static void st_moving_down(fsm_ctx_t *ctx);
static void st_dwelling(fsm_ctx_t *ctx);

static state_handler_t HANDLERS[] = {
    st_idle,
    st_moving_up,
    st_moving_down,
    st_dwelling
};


void fsm_init(fsm_ctx_t *ctx, QueueHandle_t queue) {
    ctx->state = STATE_IDLE;
    ctx->current_level = LEVEL_DOWN;
    ctx->occupied = false;
    ctx->pending_requests = queue;
    ctx->has_active_req = false;

    fsm_set_leds_for_level(LEVEL_DOWN);
    ESP_LOGI(TAG, "System startat: Hissen står på våning 0 (botten).");
}

void fsm_run_step(fsm_ctx_t *ctx) {
    HANDLERS[ctx->state](ctx);
}


static void maybe_load_new_request(fsm_ctx_t *ctx) {
    if (!ctx->has_active_req) {
        if (xQueueReceive(ctx->pending_requests, &ctx->active_req, 0)) {
            ctx->has_active_req = true;
            ESP_LOGI(TAG, "Ny passagerare: vill åka från våning %d till våning %d",
                     ctx->active_req.origin, ctx->active_req.destination);
        }
    }
}


static void st_idle(fsm_ctx_t *ctx) {
    fsm_set_leds_for_level(ctx->current_level);
    ESP_LOGI(TAG, "IDLE: Hissen väntar på våning %d.", ctx->current_level);

    maybe_load_new_request(ctx);

    if (ctx->has_active_req) {
        int origin = ctx->active_req.origin;

        if (origin > ctx->current_level) {
            ctx->state = STATE_MOVING_UP;
            ESP_LOGI(TAG, "Hissen ska upp mot våning %d.", origin);
        } else if (origin < ctx->current_level) {
            ctx->state = STATE_MOVING_DOWN;
            ESP_LOGI(TAG, "Hissen ska ner mot våning %d.", origin);
        } else {
            ctx->state = STATE_DWELLING;
            ESP_LOGI(TAG, "Passagerare finns på samma våning (%d).", origin);
        }
    }
}


static void st_moving_up(fsm_ctx_t *ctx) {
    fsm_all_leds_off();
    ESP_LOGI(TAG, "RÖRELSE: Hissen åker upp från våning %d...", ctx->current_level);
    fsm_delay_seconds(T_TRAVEL_SEC);

    ctx->current_level++;
    fsm_set_leds_for_level(ctx->current_level);
    ESP_LOGI(TAG, "Hissen stannar på våning %d.", ctx->current_level);

    if (ctx->current_level == ctx->active_req.origin && !ctx->occupied) {
        ctx->state = STATE_DWELLING;
        ESP_LOGI(TAG, "Hissen har nått passagerarens startvåning.");
    } else if (ctx->current_level == ctx->active_req.destination && ctx->occupied) {
        ctx->state = STATE_DWELLING;
        ESP_LOGI(TAG, "Hissen har nått passagerarens mål.");
    }
}


static void st_moving_down(fsm_ctx_t *ctx) {
    fsm_all_leds_off();
    ESP_LOGI(TAG, "RÖRELSE: Hissen åker ner från våning %d...", ctx->current_level);
    fsm_delay_seconds(T_TRAVEL_SEC);

    ctx->current_level--;
    fsm_set_leds_for_level(ctx->current_level);
    ESP_LOGI(TAG, "Hissen stannar på våning %d.", ctx->current_level);

    if (ctx->current_level == ctx->active_req.origin && !ctx->occupied) {
        ctx->state = STATE_DWELLING;
        ESP_LOGI(TAG, "Hissen har nått passagerarens startvåning.");
    } else if (ctx->current_level == ctx->active_req.destination && ctx->occupied) {
        ctx->state = STATE_DWELLING;
        ESP_LOGI(TAG, "Hissen har nått passagerarens mål.");
    }
}


static void st_dwelling(fsm_ctx_t *ctx) {
    ESP_LOGI(TAG, "DWELL: Dörrarna öppnas (väntar %d sek)...", T_DWELL_SEC);
    fsm_delay_seconds(T_DWELL_SEC);

    // Boarding (passagerare går in)
    if (ctx->current_level == ctx->active_req.origin && !ctx->occupied) {
        ctx->occupied = true;
        ESP_LOGI(TAG, "Passagerare går in på våning %d.", ctx->current_level);

        if (ctx->active_req.destination > ctx->current_level) {
            ctx->state = STATE_MOVING_UP;
            ESP_LOGI(TAG, "Passageraren vill åka UPP till våning %d.", ctx->active_req.destination);
        } else if (ctx->active_req.destination < ctx->current_level) {
            ctx->state = STATE_MOVING_DOWN;
            ESP_LOGI(TAG, "Passageraren vill åka NER till våning %d.", ctx->active_req.destination);
        } else {
            ESP_LOGI(TAG, "Passageraren är redan på rätt våning!");
            ctx->occupied = false;
            ctx->has_active_req = false;
            ctx->state = STATE_IDLE;
        }
    }
    // Leaving (passagerare går av)
    else if (ctx->current_level == ctx->active_req.destination && ctx->occupied) {
        ctx->occupied = false;
        ctx->has_active_req = false;
        ESP_LOGI(TAG, "Passagerare gick av på våning %d. Hissen är tom.", ctx->current_level);
        ctx->state = STATE_IDLE;
    }
}
