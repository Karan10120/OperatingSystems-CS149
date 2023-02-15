#include <stdio.h>

int main( ) {
    FILE *fp = fopen("names.txt", "r");
    if (fp == NULL) {
        printf("cannot open file\n");
        exit(1);
    }

    char line[30];
    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("Input error.\n"); exit(1);
    }

    printf("%s\n", line);

}
