#include <stdio.h>
#include <limits.h>

int main(void) {

   int userValues[20];
   int count;
   scanf("%d", &count);
   for (int i = 0; i < count; i++) {
      int input;
      scanf("%d", &input);
      userValues[i] = input;
   }

   int firstSmallest = INT_MAX;
   int secondSmallest = INT_MAX;
   for (int i = 0; i < count; i++) {
      int val = userValues[i];
      if (val < secondSmallest) {
        if (val < firstSmallest) {
            secondSmallest = firstSmallest;
            firstSmallest = val;
        }else secondSmallest = val;
     }
   }

   printf("%d and %d\n", firstSmallest, secondSmallest);
   return 0;
}
