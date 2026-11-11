
#include <stdio.h>
#include <limits.h>
#include "circular_buffer.h"



void initCircularBuffer(struct circularBuffer* bufferPtr, int* data, int maxLength) {
  bufferPtr->data = data;
  bufferPtr->head = 0;
  bufferPtr->tail = 0;
  bufferPtr->maxLength = maxLength;
  bufferPtr->count = 0;
}



int contains(struct circularBuffer* bufferPtr, int value) {
  int i = bufferPtr->head;
  int c = bufferPtr->count;

  while (c--) {
    if (bufferPtr->data[i] == value) {
      return value;
    }
    i = (i + 1) % bufferPtr->maxLength;
  }
  return INT_MIN;
}

int addElement(struct circularBuffer* bufferPtr, int value){
  
  if (bufferPtr->count == bufferPtr->maxLength) {
    return INT_MIN; 
  }



  bufferPtr->data[bufferPtr->tail] = value;
  bufferPtr->tail = (bufferPtr->tail + 1) % bufferPtr->maxLength;
  bufferPtr->count++;
  return value;
}




int removeValue(struct circularBuffer* bufferPtr, int value){
  
  if (contains(bufferPtr, value) != value){
    return INT_MIN; 
  }

  int tempArray[bufferPtr->maxLength];
  int tempArrayIndex = 0;

  //copy the values we dont want to remove to tempArray
  int i = bufferPtr->head; 
  int c = bufferPtr->count;

  while(c--) {
    if(bufferPtr->data[i] != value){
      tempArray[tempArrayIndex++] = bufferPtr->data[i];
    }
    i = (i + 1) % bufferPtr->maxLength;
  }
  
  bufferPtr->head = 0;
  bufferPtr->tail = tempArrayIndex % bufferPtr->maxLength;
  bufferPtr->count = tempArrayIndex;  // <--- update count!

  for(int x = 0; x < tempArrayIndex; x++){
    bufferPtr->data[x] = tempArray[x];  // simpler to copy to data[x]
  }
  return value; 


  
}




int removeHead(struct circularBuffer* bufferPtr){
  if (bufferPtr->count == 0) {
    return INT_MIN; 
  }
  int value = bufferPtr->data[bufferPtr->head]; 
  bufferPtr->head = (bufferPtr->head + 1) % bufferPtr->maxLength; 
  bufferPtr->count--;

  return value; 
}



void printBuffer(struct circularBuffer* bufferPtr){
  printf("Head %d\n", bufferPtr->head);
  printf("Tail %d\n", bufferPtr->tail);
  printf("Count %d\n", bufferPtr->count);


  int i = bufferPtr->head;
  int elementsPrinted = 0;

  while (elementsPrinted < bufferPtr->count) {
      printf("[%d]: %d\n", i, bufferPtr->data[i]);
      i = (i + 1) % bufferPtr->maxLength;
      elementsPrinted++;
  }
}




void addElementTest1(struct circularBuffer* bufferPtr){
  printf ("Added value %d\n", addElement(bufferPtr, 1));

}

void addElementTest2(struct circularBuffer* bufferPtr){


}

