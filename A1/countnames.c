#include <stdio.h>
#include <stdlib.h>

int main( ) {
    FILE *fp = fopen("names.txt", "r");
    if (fp == NULL) {
        printf("cannot open file\n");
        exit(1);
    }

    char line[30];
    int count = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {

        if (line[0] ==  '\n') {
            fprintf(stderr, "Warning - Line %d is empty.\n", count);
        } else {
            printf("%s", line);
        }
        count++;
    }




    fclose(fp);
    return(0);
}
