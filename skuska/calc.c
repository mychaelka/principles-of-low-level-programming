#include <stdio.h>
#include <stdlib.h>

int file_peek(FILE* file)
{
    int c = getc(file);
    return c == EOF ? EOF : ungetc(c, file);
}

int compare_int(const void* a, const void* b) {
    return (*(int *) a - *(int *) b);
}

int main(int argc, char **argv)
{
    char* input = argv[1];
    char *output = argv[2];

    FILE* in = fopen(input, "r");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    int* results = malloc(sizeof(int));
    if (results == NULL) {
        fclose(in);
        return EXIT_FAILURE;
    }

    size_t index = 0;
    size_t array_size = 0;
    while (file_peek(in) != EOF) {
        int* tmp = realloc(results, (array_size + 1) * sizeof(int));
        if (tmp == NULL) {
            free(results);
            return EXIT_FAILURE;
        }
        results = tmp;

        int result = 0;
        int left = 0;
        int right = 0;
        char op = ' ';
        // create 2D array of lines and each time save the line there
        fscanf(in, "%d %c %d\n", &left, &op, &right);

        switch (op) {
            case '+':
                result = left + right;
                break;
            case '-':
                result = left - right;
                break;
            case '*':
                result = left * right;
                break;
            case '/':
                result = left / right;
                break;
            default:
                break;
        }

        results[index] = result;
        index++;
        array_size++;
    }

    qsort(results, array_size, sizeof(int), compare_int);

    FILE* out = fopen(output, "a");
    for (size_t i = 0; i < array_size; i++) {
        fprintf(out, "%d\n", results[i]);
    }

    free(results);
    fclose(in);
    fclose(out);
    return 0;
}