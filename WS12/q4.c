/*
 * Description: OSTEP CH.14 Question 4
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 04/24/2023
 * Creation date: 04/24/2023
 *
 * There is no output, but there is a memory leak in
 * the program which is verified by running the program
 * with `valgrind --leak-check=yes`. The program runs
 * normally using gdb however.
 */
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate memory for the data array
    int *data = (int*) malloc(100 * sizeof(int));
    if (data == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    return 0;
}

