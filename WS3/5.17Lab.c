#include <stdio.h>

int main(void) {
   const int NUM_ELEMENTS = 9;
   int userValues[NUM_ELEMENTS];    // Set of data specified by the user
   int count = 0;

   for (int i = 0; i < 10; i++) {
      int input;
      scanf("%d", &input);
      if (input < 0) break;
      if (count == 9) {
         count++;
         break;
      }
      userValues[i] = input;
      count++;
   }
   if (count >= 10) printf("Too many numbers\n");
   else printf("Middle item: %d\n", userValues[count/2]);

   return 0;
}
