#include <stdio.h>

int main(void) {
   const int NUM_ELEMENTS = 20;         // Number of input integers
   int userVals[NUM_ELEMENTS];          // Array to hold the user's input integers
   int count = 0;
   int currentVal;

   while (scanf("%d", &currentVal) >= 0) {
      userVals[count] = currentVal;
      count++;
   }

   for (int i = (count -1); i >0; i--) {
      printf("%d,", userVals[i]);
   }
   printf("\n");

   return 0;
}
