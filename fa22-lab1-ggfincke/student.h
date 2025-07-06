#ifndef STUDENT_H_
#define STUDENT_H_
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int squareOfSmallest(int array[], int length);
int findMin(int *nums, int numsSize);
bool isPalindrome(char str[]);
int freqOfChar(char str[], char key);
void sort(int array[],int length);
int* twoSum(int* nums, int numsSize, int target);
int* decryptPointer(int array[], int length, int *key[]);


#endif
