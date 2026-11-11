#include "pins.h"
#include "driver/gpio.h"

/* initialises the 4 pins */
void initPins()
{
    // init the 2 LEDs pins as output and the 2 buttons' pins as input
    // you will need to use gpio_config()

    // led1 and button1 togheter and led2 and button2 together


    //led1
    gpio_config_t led1 = 
    {
    .pin_bit_mask = (uint64_t)1 << 25,
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    };
    gpio_config(&led1);


    //button1
    gpio_config_t button1 = {

    .pin_bit_mask = (uint64_t)1 << 26,
    .mode = GPIO_MODE_INPUT,
    .pull_down_en = 0,
    .pull_up_en = 1,
    };
    gpio_config(&button1);


    //led2
    gpio_config_t led2 = {
    .pin_bit_mask = (uint64_t)1 << 33,
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    };
    gpio_config(&led2);


    //button2
    gpio_config_t button2 = {
    .pin_bit_mask = (uint64_t)1 << 27,
    .mode = GPIO_MODE_INPUT,
    .pull_down_en = 0,
    .pull_up_en = 1,
    };
    gpio_config(&button2);

}

/* switches LED 1 on if level!=0 or off if level==0*/
void setLED1(uint8_t level)
{
    if (level)
    {
        // set the pin of LED 1 to ON
        // you probably need to use gpio_set_level()
        gpio_set_level(25, 1);
    }
    else
    {
        // set the pin of LED 1 to OFF
        gpio_set_level(25, 0);
    }
}

/* switches LED 2 on if level!=0 or off if level==0*/
void setLED2(uint8_t level)
{
    if (level)
    {
        // set the pin of LED 2 to ON
        // you probably need to use gpio_set_level()
        gpio_set_level(33, 1);
    }
    else
    {
        // set the pin of LED 2 to OFF
        gpio_set_level(33, 0);
    }}

/* tells if button A is currently being pressed */
uint8_t isButton1Pressed()
{
    // read the value of button 1
    // if using a pull-up, the button is pressed when the pin is LOW
    int level =  gpio_get_level(GPIO_NUM_26);

    if(!level)
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
    

}

/* tells if button A is currently being pressed */
uint8_t isButton2Pressed()
{
    // read the value of button 1
    // if using a pull-up, the button is pressed when the pin is LOW
    int level =  gpio_get_level(GPIO_NUM_27);

    if(!level)
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
}