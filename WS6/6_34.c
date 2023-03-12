#include <stdio.h>
#include <stdbool.h>

bool IsArrayMult10(int inputVals[], int numVals) {
   for (int i = 0; i < numVals; i++) {
      if (inputVals[i]%10 != 0) return false;
   }
   return true;
}

bool IsArrayNoMult10(int inputVals[], int numVals) {
   for (int i = 0; i < numVals; i++) {
      if (inputVals[i]%10 == 0) return false;
   }
   return true;
}

int main(void) {

   int size;
   scanf("%d", &size);
   int num;
   int inputVals[size];
   for (int i = 0; i < size; i++) {
      scanf("%d", &num);
      inputVals[i] = num;
   }

   if (IsArrayMult10(inputVals, size)) printf("all multiples of 10\n");
   else if (IsArrayNoMult10(inputVals, size)) printf("no multiples of 10\n");
   else printf("mixed values\n");

   return 0;
}
