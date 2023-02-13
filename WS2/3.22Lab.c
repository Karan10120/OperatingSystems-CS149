#include <stdio.h>

int main(void) {

   int first;
   int second;
   int third;
   int smallest;
   
   scanf("%d%d%d", &first, &second, &third);
   
   if (first < second) {
      smallest = first;
   } else { 
      smallest = second;
   }
   if (smallest < third) {
      printf("%d\n", smallest);
   } else {
      printf("%d\n", third);
   }

   return 0;
}

