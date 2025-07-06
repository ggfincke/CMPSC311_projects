#include "student.h"

// returns square of smallest number in the passed array
int squareOfSmallest(int array[], int length) 
{
  // min equals first array value
	int min = array[0];
	// for loop going through array and comparing values
	for (int i = 0; i < length; i++)
	{
		if (array[i] <  min)
		{
			min = array[i];
		}
	}
	// return value time value 
	return (min*min);
}

// finding minimum number in a given array
int findMin(int *nums, int numsSize)
{
  // min equals first value
	int min = nums[0];
	// for loop going through array and comparing values, switching min if there is a lower value
	for (int i = 1; i < numsSize; i++)
	{
		if (nums[i] <  min)
		{
			min = nums[i];
		} 
	}
	// return min
	return min;
}

// determining if a given string is a palindrome 
bool isPalindrome(char str[])
{
  // two vars to go through the string, one starting from end and the other from the other end 
	int l = 0;
	int r = strlen(str) - 1;

	// while loop for going through the string 
	while (l < r)
	{
		// if strings aren't equal, they aren't palindromes
		if (str[l] != str[r])
		{
			return false;
		}
		l++;
		r--;
	}
	return true;	
}

// finds frequency of a given character in a given array
int freqOfChar(char str[], char key)
{
	int count = 0;
	//for loop going through the length of str
	for (int i = 0; i < strlen(str); i++)
	{
	  //if char  equals the key, count is increased 
		if (str[i] == key)
		{
			count++;
		} 
	}
	//returning count
	return count;
}

//sorts an array
void sort(int array[],int length)
{
// using selection sort method - adapted code from CMPSC132
	int swap = 0;
	int currentIndex = 0;
	for (int i = 0; i < length-1; i++)
	{
		currentIndex = i;
		for (int j = i+1; j < length; j++)
		{
			if (array[j] < array[currentIndex])
			{
			  currentIndex = j;
			}
		}
		if (currentIndex != i)
		{
		    swap = array[i];
		    array[i] = array[currentIndex];
		    array[currentIndex] = swap;
		}
	}
	
}

// finds if two numbers add up to the sum of a third "target" number
int* twoSum(int* nums, int numsSize, int target)
{
  // for loop 1 - iterates through each number in array
  for (int i = 0; i < numsSize-1; i++)
	{
	  // for loop 2 - iterates through the other numbers and checks if it sums with i to the target 
		for (int j = i; j < numsSize; j++)
		{
		  // checking sum
		  if (nums[i] + nums[j] == target)
		  {
		    // initializing the return array and pointer
		    int retArr[2] = {nums[i],nums[j]};
		    int *retPTR;
		    retPTR = (int*)malloc(2);
		    for (int g = 0; g < 2; g++)
		      {
			retPTR[g]=retArr[g];
		      }
		    return retPTR;
		  }
                }
        }
  // returns null if nothing is found
  return NULL;
}

// finds sum of values from an array and values from the pointers to another array and returns an array of that 
int* decryptPointer(int array[], int length, int *key[])
{
  // initialize array to store the added values
  int added[length];
  for (int i = 0; i<length;i++)
    {
      added[i] = array[i] + *key[i];
    }
  // return pointer
  int *retPTR;
  retPTR = (int*)malloc(length);
  // for loop adding array spots to return pointer
  for (int j = 0; j < length; j++)
    {
      retPTR[j] = added[j];
    }
  return retPTR;
}




