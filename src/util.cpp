#include <util.h>

void clearCharArray(char* arr, int size)
{
  for (int i = 0; i < size; i++)
  {
    arr[i] = '\0';
  }
}