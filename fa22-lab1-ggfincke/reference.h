#ifndef REF_H_
#define REF_H_
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int ref_squareOfSmallest(int array[], int length);
int ref_findMin(int *nums, int numsSize);
bool ref_isPalindrome(char str[]);
int ref_freqOfChar(char str[], char key);
void ref_sort(int array[],int length);
int* ref_twoSum(int* nums, int numsSize, int target);
int* ref_decryptPointer(int array[], int length, int *key[]);

#endif
