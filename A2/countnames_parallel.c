/**
 * Description: This program counts the occurences of names in
 * multiple files parallely.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 03/06/2023
 * Creation date: 03/05/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

typedef struct my_data {
  char name[30];
  int count;
} my_data;

int main(int argc, char *argv[]) {

    //test new line
    //checks if no file or other parameters are specified
    if (argc < 2) {
        return(0);
    }
    my_data totalcounts[100]={ { '\0', 0 } };
    int nameIndex = 0;

    for (int i = 1; i < argc; i++) {
        int fd[2];
        if (pipe(fd) == -1) {
            printf("pipe failed");
            exit(0);
        }

        pid_t pid;
        pid = fork();

        if (pid < 0) {
            printf("fork failed");
            exit(0);
        } else if (pid == 0) {
            //child process
            //checks if file is inputted and opens if file exists
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                printf("cannot open file: %s\n", argv[i]);
                exit(1);
            }
            // while loop

            my_data namecounts[100]={ { '\0', 0 } };
            //line buffer
            char line[31];
            //count of current line of file on
            int lineCount = 1;
            int nameIndex = 0;

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
                    for (int j = 0; j < 100; j++) {
                    //Check for match and increments unique name occurence, then for loop breaks
                        if (strcmp(namecounts[j].name, line) == 0){
                            namecounts[j].count++;
                            found = 1;
                            break;
                        }
                    }
                    //Adds name to array if match is not found in array
                    if (found != 1) {
                        strcpy(namecounts[nameIndex].name, line);
                        namecounts[nameIndex].count = 1;
                        nameIndex++;
                    }
                }
                lineCount++;
            }
            fclose(fp);



            close(fd[0]);
            write(fd[1], &namecounts, sizeof(my_data)*100);
            close(fd[1]);

            exit(0);
        } else { //parent process
            wait(NULL);
            my_data namecounts[100]={ { '\0', 0 } };
            close(fd[1]);

            int testCount = 0;
                read(fd[0], namecounts, sizeof(my_data)*100);
                for (int k = 0; k < 100; k++) {
                    int found = 0;
                    for (int j = 0; j < nameIndex; j++) {
                        if (namecounts[k].count != 0) {
                            if (strcmp(namecounts[k].name, totalcounts[j].name) == 0){
                                totalcounts[j].count += namecounts[k].count;
                                found = 1;
                                break;
                            }
                        }
                    }

                    if (found != 1) {
                        if (namecounts[k].count != 0) {
                           strcpy(totalcounts[nameIndex].name, namecounts[k].name);
                            totalcounts[nameIndex].count = namecounts[k].count;
                            nameIndex++;
                        }

                    }
                }

        }

    }
    
    for (int w = 0; w < nameIndex; w++) {
        printf("%d %s: %d\n", w, totalcounts[w].name, totalcounts[w].count);
    }

    return(0);
}
