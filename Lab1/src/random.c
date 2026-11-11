#include "random.h"
#include "esp_random.h"


/* Returns a random number contained between min and max.
   min: minimum number of ms
   max: maximum number of ms
*/

int getRandommsecs(int min, int max){

    uint32_t randomNum = esp_random();
    float r = (float)randomNum/ (float)UINT32_MAX;
    return (int)(r*(max - min)+ min ); 
}