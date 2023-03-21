#include <stdio.h>

void SwapValues(int* userVal1, int* userVal2, int* userVal3, int* userVal4) {
   int temp1 = *userVal1;
   int temp3 = *userVal3;
   
   *userVal1 = *userVal2;
   *userVal2 = temp1;
   *userVal3 = *userVal4;
   *userVal4 = temp3;
}


int main(void) {

   int val1, val2, val3, val4;
   scanf("%d %d %d %d", &val1, &val2, &val3, &val4);
   
   SwapValues(&val1, &val2, &val3, &val4);
   
   printf("%d %d %d %d\n", val1, val2, val3, val4);

   return 0;
}
