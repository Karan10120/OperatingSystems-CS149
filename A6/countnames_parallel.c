/**
 * Description: This program counts the occurences of names in
 * multiple files parallely through multi-threading.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 05/10/2023
 * Creation date: 05/08/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//Defining my_data type structure that contains a name and the count of occurences.
typedef struct my_data {
  char name[30];
  int count;
} my_data;


/**
 * This function counts and prints names for one or more files.
 * Assumption:
 * 1. The longest name is 30 characters long.
 * 2. The file is atmost 100 lines long.
 * 3. Each line contains atmost a name.
 * 4. Each name is separated by a new line.
 * 5. Names contain simple characters of Ascii values from 0-128.
 * 6. Input file contains only valid characters.
 *
 * Input parameters: argc, argv[]
 * Returns: int
 * Prints: "Names: Occurences"
**/

int main(int argc, char *argv[]) {

    //checks if no file or other parameters are specified
    if (argc < 2) {
        return(0);
    }
    else if (argc > 3) {
        fprintf(stderr, "Too many files inputted, please only pass 2 input file.\n");
        return(0);
    }
    my_data totalcounts[100]= {0};
    int nameIndex = 0;

    //Loops through each file inputted to create pipe and fork child process that counts name occurences.
    for (int i = 1; i < argc; i++) {
        int fd[2];
        //Instantiate pipe and checks if creation of pipe fd fails and exits.
        if (pipe(fd) == -1) {
            printf("pipe failed");
            exit(0);
        }
        pid_t pid;
        pid = fork();

        //This check if the fork fails, if the fork fails it exits program.
        if (pid < 0) {
            printf("fork failed");
            exit(0);
        } else if (pid == 0) {
            //Enter child process
            FILE *fp = fopen(argv[i], "r");
            //checks if file is inputted and opens if file exists
            if (fp == NULL) {
                printf("cannot open file: %s\n", argv[i]);
                exit(1);
            }
            //creates array of 100 my_data types and initalizes it to null and 0
            my_data namecounts[100]={0};
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

            close(fd[0]);      //Close reading end of the pipe
            write(fd[1], &namecounts, sizeof(my_data)*100);   //Write namecounts array to pipe
            close(fd[1]);                      //Close writing end of pipe

            exit(0);
        } else {
            //Entering parent process
            wait(NULL);        //Wait for children to terminate
            my_data namecounts[100]={0};
            close(fd[1]);       //Close writing end of pipe

            //Reads from pipe buffer the most recent file in consecutive order
            read(fd[0], namecounts, sizeof(my_data)*100);
            //This loops over the array read from the pipe
            for (int k = 0; k < 100; k++) {
                int found = 0;
                //This loops through the previously seen names
                for (int j = 0; j < nameIndex; j++) {
                    //Checks to only read non-empty entries
                    if (namecounts[k].count != 0) {
                        //Checks if name was seen before and sums counts together
                        if (strcmp(namecounts[k].name, totalcounts[j].name) == 0){
                            totalcounts[j].count += namecounts[k].count;
                            found = 1;       //Name was seen before
                            break;
                        }
                    }
                }
                //Name was not seen before
                if (found != 1) {
                    //Checks to only read non-empty entries
                    if (namecounts[k].count != 0) {
                        //Copys new name from read array to total count array.
                        strcpy(totalcounts[nameIndex].name, namecounts[k].name);
                        totalcounts[nameIndex].count = namecounts[k].count;
                        nameIndex++;
                    }
                }
            }
        }
    }
    //This loops through the aggregated counts and prints out the number of occurences of each name.
    for (int w = 0; w < nameIndex; w++) {
        printf("%s: %d\n", totalcounts[w].name, totalcounts[w].count);
    }

    return(0);
}
