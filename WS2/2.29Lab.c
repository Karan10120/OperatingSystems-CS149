#include <stdio.h>
#include <math.h>

int main(void) {
   double x;
   double y;
   double z;

   scanf("%lf%lf%lf", &x, &y, &z);
   
   printf("%0.2lf ", pow(x, z));
   printf("%0.2lf ", pow(x, pow(y, 2)));
   printf("%0.2lf ", fabs(y));
   printf("%0.2lf\n", sqrt(pow(x*y, z)));

   return 0;
}

