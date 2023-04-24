/*
 * There is an invalid write since the array is of size 100,
 * but does not have an index of 100. Running the program with
 * valgrind returns a memory error since the array was not
 * freed. In order to correct the program, we would need to set 
 * the 99th index to zero and also free the array after using it.
 *
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

    data[100] = 0;

    return 0;
}

