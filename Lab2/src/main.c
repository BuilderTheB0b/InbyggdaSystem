#include <stdio.h>
#include <stdlib.h>
#include <esp32/rom/ets_sys.h>
#include "circular_buffer.h"

/*
 * Change the value of BUFFER_SIZE if you want to change the size of the buffer. 
 */ 
#define BUFFER_SIZE 5

void test1(struct circularBuffer* bufferPtr, int elementToAdd);
void test2(struct circularBuffer* bufferPtr, int elementToAdd1, int elementToAdd2);
void test3(struct circularBuffer* bufferPtr);
void test4(struct circularBuffer* bufferPtr);
void test6(struct circularBuffer* bufferPtr);
void test7(struct circularBuffer* bufferPtr, int elementToAdd);
void test8(struct circularBuffer* bufferPtr, int elementToAdd1, int elementToAdd2);
void test9(struct circularBuffer* bufferPtr, int lastElementToAdd);







void app_main() {
    

    // Some code to help you get started
    struct circularBuffer buffer;
    int *buffer_data = (int*) malloc(BUFFER_SIZE * sizeof(int));
    initCircularBuffer(&buffer, buffer_data, BUFFER_SIZE);

    printf("Max length = %d\n", BUFFER_SIZE);
    printf("Head position = %d\n", buffer.head);
    printf("Tail position = %d\n", buffer.tail);



    test1(&buffer, 5);

    test2(&buffer, 7, 3);

    test3(&buffer);

    test4(&buffer);

    test6(&buffer);

    test7(&buffer, 11);

    test8(&buffer, 4, 7);
    initCircularBuffer(&buffer, buffer_data, BUFFER_SIZE);

    test9(&buffer, 99);

    addElementTest2(&buffer);






    

    free(buffer_data);

    }

    void test1(struct circularBuffer* bufferPtr, int elementToAdd){
        printf ("Test1\n");
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd));

        printf("Should remove one element\n ");
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printBuffer(bufferPtr);
        printf("\n");


    }

    void test2(struct circularBuffer* bufferPtr, int elementToAdd1, int elementToAdd2){
        printf ("Should add two values\n");
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd1));
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd2));

        printf("Should remove two elements\n ");
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
       
        printBuffer(bufferPtr);
        printf("\n");


    }

    void test3(struct circularBuffer* bufferPtr){
        printf ("Test3\n");
        printf ("Added value %d\n", addElement(bufferPtr, 1));
        printf ("Added value %d\n", addElement(bufferPtr, 2));
        printf ("Added value %d\n", addElement(bufferPtr, 3));
        printf ("Added value %d\n", addElement(bufferPtr, 4));
        printf ("Added value %d\n", addElement(bufferPtr, 5));

        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));

        printBuffer(bufferPtr);
        printf("\n");


    }
    //Does both test 4 and 5
    void test4(struct circularBuffer* bufferPtr){
        printf ("Test4\n");
        printf ("Added value %d\n", addElement(bufferPtr, 1));
        printf ("Added value %d\n", addElement(bufferPtr, 2));
        printf ("Added value %d\n", addElement(bufferPtr, 3));
        printf ("Added value %d\n", addElement(bufferPtr, 4));
        printf ("Added value %d\n", addElement(bufferPtr, 5));
        printf ("Added value %d\n", addElement(bufferPtr, 6));

        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));
        printf ("Removed value %d\n", removeHead(bufferPtr));

        printBuffer(bufferPtr);
        printf("\n");




    }

    void test6(struct circularBuffer* bufferPtr){
        printf ("Buffer contains %d\n", contains(bufferPtr, 1));

        printBuffer(bufferPtr);
        printf("\n");

    }

    void test7(struct circularBuffer* bufferPtr, int elementToAdd){
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd));
        printf ("Buffer contains %d\n", contains(bufferPtr, elementToAdd));

        printBuffer(bufferPtr);
        printf("\n");

    }

    void test8(struct circularBuffer* bufferPtr, int elementToAdd1, int elementToAdd2){
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd1));
        printf ("Added value %d\n", addElement(bufferPtr, elementToAdd2));

        printf ("Buffer contains %d\n", contains(bufferPtr, elementToAdd2));

        printBuffer(bufferPtr);
        printf("\n");

    }

    void test9(struct circularBuffer* bufferPtr , int lastElementToAdd){
        printf ("Test9\n");

        printf ("Added value %d\n", addElement(bufferPtr, 1));
        printf ("Added value %d\n", addElement(bufferPtr, 2));
        printf ("Added value %d\n", addElement(bufferPtr, 3));
        printf ("Added value %d\n", addElement(bufferPtr, 4));
        printf ("Added value %d\n", addElement(bufferPtr, lastElementToAdd));

        printf ("Buffer contains %d\n", contains(bufferPtr, lastElementToAdd));
        printBuffer(bufferPtr);

        printf("\n");


    }


