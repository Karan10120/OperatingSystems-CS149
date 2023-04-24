/*
 * The program runs and prints out a zero for the array
 * element. When using valgrind, it gives us an invalid
 * read error. This is expected since we already
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

