
#include "circular_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int i;
int overwritedElements = 0;

void initCircularBuffer(struct circularBuffer *bufferPtr, int *data, int maxLength)
{
  bufferPtr->data = data;
  bufferPtr->head = 0;  // Initialize head to 0
  bufferPtr->tail = -1; // Initialize tail to -1 (empty buffer)
  bufferPtr->maxLength = maxLength;
  bufferPtr->usedElements = 0;
  
}

int contains(struct circularBuffer *bufferPtr, int value)
{
  int counter = 0; // Initialize counter to 0
  for (i = 0; i < bufferPtr->maxLength; i++)
  {
    if (bufferPtr->data[i] == value)
    {
      counter++;
    }
  }
  if (counter > 0)
  {
    return counter;
  }
  else
  {
    return INT_MIN;
  }
}

int addElement(struct circularBuffer *bufferPtr, int value)
{
  if (bufferPtr->usedElements < bufferPtr->maxLength)
  {
    bufferPtr->tail = (bufferPtr->tail + 1) % bufferPtr->maxLength;
    bufferPtr->data[bufferPtr->tail] = value;
    bufferPtr->usedElements++;
    return value;
  }
  else
  {
    printf("Unfortunately, our buffer is full. Cannot add more elements: %d\n", value);
    return -1;
  }
}

int removeValue(struct circularBuffer *bufferPtr, int value)
{
  if (bufferPtr->usedElements == 0)
  {
    return INT_MIN;
  }
  else
  {
    int antalElements = 0;

    for (int i = 0; i < bufferPtr->maxLength; i++)
    {
      if (bufferPtr->data[i] != INT_MIN && bufferPtr->data[i] != value)
      {
        antalElements++;
      }
    }

    int *antalArray = malloc(antalElements * sizeof(int));
    int position = bufferPtr->head;
    int raknaren = 0;

    while (raknaren < antalElements)
    {
      if (bufferPtr->data[position] != INT_MIN && bufferPtr->data[position] != value)
      {
        antalArray[raknaren] = bufferPtr->data[position];
        raknaren++;
      }
      position = (position + 1) % bufferPtr->maxLength;
    }

    for (int i = 0; i < bufferPtr->maxLength; i++)
    {
      bufferPtr->data[i] = INT_MIN;
    }

    position = bufferPtr->head;

    for (int i = 0; i < antalElements; i++)
    {
      bufferPtr->data[position] = antalArray[i];
      position = (position + 1) % bufferPtr->maxLength;
    }

    bufferPtr->tail = (position - 1 + bufferPtr->maxLength) % bufferPtr->maxLength;
    free(antalArray);
    bufferPtr->usedElements -= antalElements;

    return value;
  }
}

int removeHead(struct circularBuffer *bufferPtr)
{
  int value;
  if (bufferPtr->usedElements == 0)
  {
    printf("Could not remove head, buffer is empty\n");
    return INT_MIN;
  }
  else if (bufferPtr->usedElements == 1)
  {
    value = bufferPtr->data[bufferPtr->head];
    bufferPtr->data[bufferPtr->head] = INT_MIN;
    bufferPtr->usedElements--;
    return value;
  }
  else
  {
    value = bufferPtr->data[bufferPtr->head];
    bufferPtr->data[bufferPtr->head] = INT_MIN;
    bufferPtr->head = (bufferPtr->head + 1) % bufferPtr->maxLength;
    bufferPtr->usedElements--;
    return value;
  }
}
u_int32_t getsize(struct circularBuffer* bufferPtr) { 
    return bufferPtr->usedElements; 
}
int modulus_inc(int maxLength, int val) { 
    return (val + 1) % maxLength; 
}

void printBuffer(struct circularBuffer *bufferPtr)
{
  int startPos = bufferPtr->head;
  int printedElements = 0; // The number of printed elements
  while (printedElements < bufferPtr->usedElements)
  {
    printf("Element at index %d: %d\n", startPos, bufferPtr->data[startPos]);
    startPos = (startPos + 1) % bufferPtr->maxLength;
    printedElements++;
  }
}