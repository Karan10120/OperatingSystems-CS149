#include <stdio.h>
#include <string.h>

int main()
{
   char line[51];
   int count = 0;
   
   fgets(line, sizeof(line), stdin);
   
   for (int i = 0; i < strlen(line); i++) {
      char c = line[i];
      if (c != ' ' && c != '.' && c != '!' && c != ',') {
         count++;
      }
   }
   
   printf("%d\n", count);
   return 0;
}
