
#include <stdint.h>
#include <stdio.h>
#include <esp_task_wdt.h>
#include "pins.h"
#include "random.h"

/*
  Waits for "millis" milliseconds without upsetting the watchdog timer
*/
void waitMs(unsigned int millis)
{
    TickType_t delay = millis / portTICK_PERIOD_MS;
    vTaskDelay(delay);
}

void app_main()
{
    initPins();
    uint8_t winner = 0;


    while (1)
    {
    
        // Game starts
        setLED1(1);
        setLED2(1);
        waitMs(1000);
        setLED1(0);
        setLED2(0);
        waitMs(1000);
        setLED1(1);
        setLED2(1);
        waitMs(1000);
        //Switch both leds off
        setLED1(0);
        setLED2(0);

        // get a random duration between 3 and 5 seconds
        int random = getRandommsecs(3,5);
        
        // wait that random duration
        waitMs (random*1000);

        int cheat = 0;

        if (isButton1Pressed()){
            winner = 2;
            cheat = 1; 
        }
        if(isButton2Pressed()){
            cheat = 1;
            winner = 1;
        }

        if (cheat == 0){
            // switch both LEDs ON
            setLED1(1);
            setLED2(1);


            while (!winner)
            {
                // check if either button A or B are pressed
                if (isButton1Pressed())
                {
                    winner = 1;
                }
                else if (isButton2Pressed())
                {
                    winner = 2; 
                }
                // if any is pressed, set winner to 1 for A or 2 for B
            }
        }
    
    
        if (winner == 1)
        {
            setLED1(1);
            setLED2(0);
            waitMs(200);
            setLED1(0);
            waitMs(200);
            setLED1(1);
            waitMs(200);
            setLED1(0);
            waitMs(200);
            setLED1(1);
            waitMs(200);
            setLED1(0);
            setLED1(1);
            setLED2(0);
            waitMs(200);
            setLED1(0);
            waitMs(200);
            setLED1(1);
            waitMs(200);
            setLED1(0);
            waitMs(200);
            setLED1(1);
            waitMs(200);
            setLED1(0);

        }
        else if (winner == 2)
        {
            setLED2(1);
            setLED1(0);
            waitMs(200);
            setLED2(0);
            waitMs(200);
            setLED2(1);
            waitMs(200);
            setLED2(0);
            waitMs(200);
            setLED2(1);
            waitMs(200);
            setLED2(0);
            setLED2(1);
            setLED1(0);
            waitMs(200);
            setLED2(0);
            waitMs(200);
            setLED2(1);
            waitMs(200);
            setLED2(0);
            waitMs(200);
            setLED2(1);
            waitMs(200);
            setLED2(0);
        }

        // if A wins, flash LED A and switch off B
        // if B wins, flash LED B and switch off A

        // switch off both A and B and wait for some time to restart the game
        waitMs(3000);
        winner = 0;
    }


        
    
}