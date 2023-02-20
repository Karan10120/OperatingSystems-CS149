#include <stdio.h>
#include <string.h>

int main(void) {
   char stringInputs[20][10];

   int count;
   scanf("%d", &count);
   for (int i = 0; i < count; i++){
      char input[10];
      scanf("%s", input);
      strcpy(stringInputs[i], input);
   }

   char searchCharacter;
   scanf(" %c", &searchCharacter);

   for (int i = 0; i < count; i++){
      if (strchr(stringInputs[i], searchCharacter) != NULL) {
         printf("%s,", stringInputs[i]);
      }
   }
   printf("\n");
   return 0;
}
