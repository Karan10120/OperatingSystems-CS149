#include <stdio.h>

int main(void) {

   int userValues[20];
   int count;
   scanf("%d", &count);
   for (int i = 0; i < count; i++) {
      int input;
      scanf("%d", &input);
      userValues[i] = input;
   }

   int lowerBound;
   int upperBound;
   scanf("%d%d", &lowerBound, &upperBound);
   for (int i = 0; i < count; i++) {
      if (lowerBound <= userValues[i] && userValues[i] <= upperBound) {
         printf("%d,", userValues[i]);
      }
   }
   printf("\n");
   return 0;
}
