#include <stdio.h>

int MaxNumber(int num1, int num2, int num3, int num4){
   int max = num1;
   if (num2 > max) max = num2;
   if (num3 > max) max = num3;
   if (num4 > max) max = num4;
   return max;
}

int MinNumber(int num1, int num2, int num3, int num4){
   int min = num1;
   if (num2 < min) min = num2;
   if (num3 < min) min = num3;
   if (num4 < min) min = num4;
   return min;
}

int main(void) {
   int num1, num2, num3, num4;

   scanf("%d%d%d%d", &num1, &num2, &num3, &num4);
   printf("Maximum is %d\n",  MaxNumber(num1, num2, num3, num4));
   printf("Minimum is %d\n",  MinNumber(num1, num2, num3, num4));
   
   return 0;
}
