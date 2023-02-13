#include <stdio.h>

int main(void) {

   int userNum;
   int x;
   int first;
   int second;
   int third;
   
   scanf("%d%d", &userNum, &x);
   first = userNum / x;
   second = first / x;
   third = second /x;
   printf("%d %d %d\n", first, second, third);

   return 0;
}

