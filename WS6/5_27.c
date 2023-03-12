#include <stdio.h>
#include <string.h>

int main(void) {

   char line[50];
   while (fgets(line, sizeof(line), stdin)) {
      line[strcspn(line, "\n")] = '\0';
      if ((strcmp(line, "Done") == 0)  || (strcmp(line, "done") == 0) || (strcmp(line, "d") == 0)) {
         break;
      }
      int size = 0;
      for (int i = 0; i < strlen(line); i++) {
         if (line[i] != '\0' && line[i] != '\n') {
            size++;
         } else break;
      }


      for (int i = 0; i < size/2; i ++) {
         char temp = line[i];
         line[i] = line[(size-1) -i];
         line[(size-1) -i] = temp;
      }

      printf("%s\n", line);
   }

   return 0;
}
