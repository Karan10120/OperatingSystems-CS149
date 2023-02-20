#include <stdio.h>

int main(void) {
   const int NUM_ELEMENTS = 20;
   int userValues[NUM_ELEMENTS];    // Set of data specified by the user

   int count;
   scanf("%d", &count);
   for (int i = 0; i < count; i++) {
      int input;
      scanf("%d", &input);
      userValues[i] = input;
   }

   int threshold;
   scanf("%d", &threshold);
   for (int i = 0; i < count; i++) {
     if (userValues[i] <= threshold) printf("%d,", userValues[i]);
   }
   printf("\n");

   return 0;
}
