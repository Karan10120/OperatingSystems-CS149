#include <stdio.h>

int main(void) {
   int currentPrice;
   int lastMonthsPrice;
   double monthlyMortgage;
   
   scanf("%d", &currentPrice);
   scanf("%d", &lastMonthsPrice);
   
   monthlyMortgage = (currentPrice * 0.051) / 12;
   
   printf("This house is $%d. The change is $%d since last \
month.\n", currentPrice, currentPrice - lastMonthsPrice);
   printf("The estimated monthly mortgage is $%f.\n", monthlyMortgage);

   return 0;
}

