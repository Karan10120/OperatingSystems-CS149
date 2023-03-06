#include <stdio.h>
#include <string.h>
#include <ctype.h>

void RemoveNonAlpha(char userString[], char userStringAlphaOnly[]) {
   int pos = 0;
   for (int i = 0; i < strlen(userString); i++) {
      char c = userString[i];
      if (c >= 'a' && c <= 'z') {
         userStringAlphaOnly[pos] = c;
         pos++;
      } else if (c >= 'A' && c <= 'Z') {
         userStringAlphaOnly[pos] = c;
         pos++;
      }
   }
   userStringAlphaOnly[pos] = '\0';
}

int main(void) {
   char line[51];
   char alphaOnly[51];
   
   fgets(line, sizeof(line), stdin);
   RemoveNonAlpha(line, alphaOnly);

   printf("%s\n", alphaOnly);
   return 0;
}

