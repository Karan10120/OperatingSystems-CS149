/**
 * Description: This program counts the occurences of names in a file.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 02/16/2023
 * Creation date: 02/14/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This function counts and prints names from a file.
 * Assumption:
 * 1. The longest name is 30 characters long.
 * 2. The file is atmost 100 lines long.
 * 3. Each line contains atmost a name.
 * 4. Each name is separated by a new line.
 * 5. Names contain simple characters of Ascii values from 0-128.
 * 6. Input file contains only valid characters.
 *
 * Input parameters: file1
 * Returns: int
 * Prints: "Names: Occurences"
**/

int main(int argc, char *argv[]) {

    //checks if file is inputted and opens if file exists
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("cannot open file\n");
        exit(1);
    }
    //define 100x30 array of characters and integers, integer count to keep track of array fill size
    char names [100][31];
    int counts [100];
    int nameCount = 0;

    //line buffer
    char line[31];
    //count of current line of file on
    int lineCount = 1;

    //This while loop iterates through each line of the file
    while (fgets(line, sizeof(line), fp) != NULL) {
        //The if else block determines whether the current line in the file is blank or contains a name
        if (line[0] ==  '\n') {
            fprintf(stderr, "Warning - Line %d is empty.\n", lineCount);
        } else {
            //Removes trailing new line character from string
            line[strcspn(line, "\n")] = 0;
            int found = 0;

            //This for loop loops through the array name until a match is seen
            for (int i = 0; i < 100; i++) {
                //Check for match and increments unique name occurence, then for loop breaks
                if (strcmp(names[i], line) == 0){
                    counts[i]++;
                    found = 1;
                    break;
                }
            }
            //Adds name to array if match is not found in array
            if (found != 1) {
                strcpy(names[nameCount], line);
                counts[nameCount] = 1;
                nameCount++;
            }
        }
        lineCount++;
    }
    fclose(fp);

    //This for loop prints the occurences of each name found in file
    for (int i = 0; i < nameCount; i++) {
        printf("%s: %d\n", names[i], counts[i]);
    }

    return(0);
}
