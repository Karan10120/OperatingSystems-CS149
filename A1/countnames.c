#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    //checks if file is inputted and opens if file exists
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("cannot open file\n");
        exit(1);
    }
    //define 100x30 array
    char names [100][31];
    int counts [100];
    int nameCount = 0;

    //reads through each line of file and gives warning if empty line of file
    char line[31];
    int lineCount = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] ==  '\n') {
            fprintf(stderr, "Warning - Line %d is empty.\n", lineCount);
        } else {
            //adding array
            int found = 0;
            for (int i = 0; i < 100; i++) {
                if (strcmp(names[i], line) == 0){
                    counts[i]++;
                    found = 1;
                    break;
                }
            }
            if (found != 1) {
                strcpy(names[nameCount], line);
                //names[nameCount] = line;
                counts[nameCount] = 1;
                nameCount++;
            }
        }
        lineCount++;
    }
    fclose(fp);

    //printing the count of each name found in file
    for (int i = 0; i < nameCount; i++) {
        printf("%s: %d\n", names[i], counts[i]);
    }

    return(0);
}
