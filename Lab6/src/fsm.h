

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


#define LED_PIN_LEVEL_UP     12
#define LED_PIN_LEVEL_MIDDLE 14
#define LED_PIN_LEVEL_DOWN   27
#define BUTTON_PIN           26


#define T_TRAVEL_SEC 5   // 5 sekunder mellan våningar
#define T_DWELL_SEC  5   // 5 sekunder för dörrar


typedef enum {
    LEVEL_DOWN = 0,
    LEVEL_MIDDLE = 1,
    LEVEL_UP = 2
} level_t;

typedef enum {
    STATE_IDLE = 0,
    STATE_MOVING_UP,
    STATE_MOVING_DOWN,
    STATE_DWELLING
} state_t;

typedef struct {
    int origin;
    int destination;
} request_t;

typedef struct {
    state_t state;               
    level_t current_level;       
    bool occupied;               
    bool has_active_req;         
    request_t active_req;        
    QueueHandle_t pending_requests; 
} fsm_ctx_t;

typedef void (*state_handler_t)(fsm_ctx_t *ctx);

void fsm_init(fsm_ctx_t *ctx, QueueHandle_t queue);
void fsm_run_step(fsm_ctx_t *ctx);


