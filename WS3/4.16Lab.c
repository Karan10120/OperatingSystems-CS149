#include <stdio.h>

int main(void) {

   int input;
   scanf("%d", &input);
   float sum = 0;
   int count = 0;
   int max = 0;

   while(input >= 0){
      sum += input;
      count++;
      if (input > max) max = input;
      scanf("%d", &input);
   }
   printf("%d %0.2lf\n", max, sum / count);
   return 0;
}
