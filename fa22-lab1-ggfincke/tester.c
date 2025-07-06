#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "student.h"
#include "reference.h"



#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// Unit test functions.
int test_squareOfSmallest();
int test_findMin();
int test_isPalindrome();
int test_freqOfChar();
int test_sort();
int test_twoSum();
int test_decryptPointer();

// Utility functions used by the unit tests.
bool arrays_differ(int arrray1[], int array2[], int length);
char *array_to_string(int array[], int length);

int main() {
  int score = 0;
  
  score += test_squareOfSmallest();
  score += test_findMin();
  score += test_isPalindrome();
  score += test_freqOfChar();
  score += test_sort();
  score += test_twoSum();
  score += test_decryptPointer();

  printf("Total score: %d/%d\n", score, 10);
}

bool arrays_differ(int array1[], int array2[], int length) {
  for (int i = 0; i < length; ++i)
    if (array1[i] != array2[i])
      return true;
  return false;
}

char *array_to_string(int array[], int length) {
  char *p = (char *)malloc(length * 10);
  for (int i = 0, n = 0; i < length; ++i) {
    n += sprintf(p + n, "%d ", array[i]);
  }
  return p;
}


int test_squareOfSmallest() {
  {
    printf("running %s: single-element-array: ", __func__);

    int in[] = {-1234};
    int out = squareOfSmallest(in, ARRAY_SIZE(in));
    int expected = ref_squareOfSmallest(in, ARRAY_SIZE(in));

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");
  }
  {
    printf("running %s: regular-array: ", __func__);

    int in[] = {123, 135, -1234, 0, 55};
    int out = squareOfSmallest(in, ARRAY_SIZE(in));
    int expected = ref_squareOfSmallest(in, ARRAY_SIZE(in));

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");
  }
  return 1;
}

int test_findMin() {
    printf("running %s ", __func__);
    int in[] = {3,4,5,1,2};
    int out = findMin(in, ARRAY_SIZE(in));
    int expected = ref_findMin(in, ARRAY_SIZE(in));

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");

  return 1;
}

int test_isPalindrome() {
  
    printf("running %s: regular-array: ", __func__);
    char in[]  = "abba";
    bool out = isPalindrome(in);
    bool expected = ref_isPalindrome(in);

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");
  
  return 1;
}

int test_freqOfChar() {
  {
    printf("running %s: array with char: ", __func__);

    char in[] = {'h','e','l','l','o','w','o','r','l','d'};
    char out = freqOfChar(in, 'l');
    char expected = ref_freqOfChar(in, 'l');

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");
  }
  {
    printf("running %s: array without char: ", __func__);

    char in[] = {'n','o','c','h','a','r'};
    char out = freqOfChar(in, 'l');
    char expected = ref_freqOfChar(in,  'l');

    if (out != expected) {
      printf("failed:\n  got: %d\n  expected: %d\n", out, expected);
      return 0;
    }
    printf("passed\n");
  }
  return 1;
}

int test_sort() {
  {
    printf("running %s: empty-array: ", __func__);
    int out[]      = {};
    int expected[] = {};
    sort(out, ARRAY_SIZE(out));
    ref_sort(expected, ARRAY_SIZE(expected));

    if (arrays_differ(out, expected, ARRAY_SIZE(out))) {
      char *out_s = array_to_string(out, ARRAY_SIZE(out));
      char *expected_s = array_to_string(expected, ARRAY_SIZE(expected));

      printf("failed:\n  got: %s\n  expected: %s\n", out_s, expected_s);

      free(out_s);
      free(expected_s);
      return 0;
    }
    printf("passed\n");
  }
  {
    printf("running %s: regular-array: ", __func__);
    int out[]      = {8, 2, 2, 3234, 234, 234, 1, 5, -15, 32, 9898};
    int expected[] = {8, 2, 2, 3234, 234, 234, 1, 5, -15, 32, 9898};

    sort(out, ARRAY_SIZE(out));
    ref_sort(expected, ARRAY_SIZE(expected));

    if (arrays_differ(out, expected, ARRAY_SIZE(out))) {
      char *out_s = array_to_string(out, ARRAY_SIZE(out));
      char *expected_s = array_to_string(expected, ARRAY_SIZE(expected));
      printf("failed:\n  got: %s\n  expected: %s\n", out_s, expected_s);
      free(out_s);
      free(expected_s);
      return 0;
    }
    printf("passed\n");
  }
  {
    printf("running %s: sorted-array: ", __func__);
    int out[]      = {-5, -1, 2, 3, 7, 198, 28349};
    int expected[] = {-5, -1, 2, 3, 7, 198, 28349};

    sort(out, ARRAY_SIZE(out));
    ref_sort(expected, ARRAY_SIZE(expected));

    if (arrays_differ(out, expected, ARRAY_SIZE(out))) {
      char *out_s = array_to_string(out, ARRAY_SIZE(out));
      char *expected_s = array_to_string(expected, ARRAY_SIZE(expected));

      printf("failed:\n  got: %s\n  expected: %s\n", out_s, expected_s);

      free(out_s);
      free(expected_s);
      return 0;
    }
    printf("passed\n");
  }
  return 2;
}

int test_twoSum() {
  {
    printf("running %s: empty-array: ", __func__);
    int out[]      = {2,7,11,15};
    int expected[] = {2,7,11,15};
    int target = 18;

    int *got = twoSum(out, 4, target);
    int *expected1 = ref_twoSum(expected, 4, target);
    if(got == NULL){
    printf("failed: returned value cannot be NULL\n");
    return 0;
    }
    if (*got != *expected1) {
      printf("failed:\n  got: %d and %d\n  expected: %d and %d\n", got[0], got[1], expected1[0], expected1[1]);
    }
    printf("passed\n");
  }
  {
    printf("running %s: empty-array: ", __func__);
    int out[]      = {1,14,3,8,16,5};
    int expected[] = {1,14,3,8,16,5};
    int target = 30;

    int *got = twoSum(out, 6, target);
    int *expected1 = ref_twoSum(expected, 6, target);
    if(got == NULL){
    printf("failed: returned value cannot be NULL\n");
    return 0;
    }
    if (*got != *expected1) {
      printf("failed:\n  got: %d and %d\n  expected: %d and %d\n", got[0], got[1], expected1[0], expected1[1]);
    }
    printf("passed\n");
  }
  return 2;
}



int test_decryptPointer(){
    printf("running %s: same key length: ", __func__);

    int key_val[] = {2, 6, 18, 4, 20, 3};
    int *key[ARRAY_SIZE(key_val)];
    for (int i = 0; i < ARRAY_SIZE(key_val); i++){
      key[i] = &key_val[i];
    }

    int in[] = {5, 10, 15, 20, 15, 30};
    int *out = decryptPointer(in, ARRAY_SIZE(in), key);
    int *expected = ref_decryptPointer(in, ARRAY_SIZE(in), key);
    
    if(out == NULL){
    printf("failed: returned value cannot be NULL\n");
    return 0;
    }

    for (int j = 0; j < ARRAY_SIZE(in); j++){
      if (out[j] != expected[j]){
        printf("failed:\n  at position %d in array...\n got: %d\n  expected: %d\n", j, out[j], expected[j]);
        return 0;
      }
    }

    printf("passed\n");

    return 2;
}





