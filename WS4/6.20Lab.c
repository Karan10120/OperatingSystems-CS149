#include <stdio.h>

double LapsToMiles(double userLaps) {
   return userLaps * 0.25;
}

int main(void) {

   double userLaps;
   scanf("%lf", &userLaps);

   
   double studentResults = LapsToMiles(userLaps);
   printf("%0.2lf\n", studentResults);
   
   return 0;
}
