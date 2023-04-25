

/*
 * Description: OSTEP CH.14 Question 6
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 04/24/2023
 * Creation date: 04/24/2023
 *
 * When the program is run, we are given a random value. However, when 
 * the program is run with valgrind, it prints out a zero for the array
 * element and it gives us an invalid read error or size 4, while there are no leaks
 * all the blocks were freed. This is expected since we already
 * freed the array before accessing an element that was in it.
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
    free(data);
    printf("array[0]: %d\n", data[0]);

    return 0;
}

