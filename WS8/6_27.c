#include <stdio.h>
#include <string.h>

int CalcNumCharacters(char* userString, char userChar) {
    int count = 0;
    for (int i = 0; i < strlen(userString); i++){
         if (userString[i] == userChar) {
            count++;
         }
    }
    return count;
}

int main(void) {
   char userChar;
   char userString[50];

   scanf("%c %s", &userChar, userString);
   int count = CalcNumCharacters(userString, userChar);
   if (count == 1) {
      printf("%d %c\n", count, userChar);
   }else {
      printf("%d %c's\n", count, userChar);
   }

   return 0;
}
