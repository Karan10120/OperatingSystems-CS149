#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char input_string[100];
    char first_word[50];
    char second_word[50];
    char *comma_ptr;

    while (1) {
        printf("Enter input string:\n");
        fgets(input_string, sizeof(input_string), stdin);

        //user wants to quit
        if (input_string[0] == 'q') {
            break;
        }

        //find the comma
        comma_ptr = strchr(input_string, ',');
        if (comma_ptr == NULL) {
            printf("Error: No comma in string.\n\n");
            continue;
        }

        //extract the first and second words
        sscanf(input_string, "%[^,],%s", first_word, second_word);

        //remove any spaces from first word
        int j = 0;
        for (int i = 0; first_word[i]; i++) {
            if (!isspace(first_word[i])) {
                first_word[j++] = first_word[i];
            }
        }
        first_word[j] = '\0';

        //remove any spaces from second word
        j = 0;
        for (int i = 0; second_word[i]; i++) {
            if (!isspace(second_word[i])) {
                second_word[j++] = second_word[i];
            }
        }
        second_word[j] = '\0';

        // Print the words
        printf("First word: %s\n", first_word);
        printf("Second word: %s\n\n", second_word);
    }

    return 0;
}

