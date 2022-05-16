char *read_line(void)
{
    size_t size = sizeof(char);
    size_t NUM_CHARACTERS = 1;

    char * line = malloc(size);
    memset(line, 0, size);

    int ch = getchar();
    size_t count = 0;
    size_t i = 0;

    while (ch != '\n' && ch != '\0' && ch != EOF) {
        if (count >= NUM_CHARACTERS) {
	    char *tmp_line = line;
	    line = realloc(line, 2 * size);
	    if (line == NULL) {
		free(tmp_line);
		return NULL;
	    }
            memset(line + size, 0, size);
            size *= 2;
            count = 0;
        }
        line[i] = (char) ch;
        count++;
        i++;
        ch = getchar();
    }

    line[i] = '\0';

    return line;
}


int dyn_free2d(void **memory, size_t rows)
{
    if (rows == 0 || memory == NULL) {
        return 1;
    }

    for (size_t i = 0; i < rows; i++) {
        free(memory[i]);
    }

    free(memory);

    return 0;
}


void **dyn_alloc2d(size_t rows, const size_t row_sizes[rows])
{
    if (rows == 0 || row_sizes == NULL) {
        return NULL;
    }

    void ** dynamic_2D = malloc(rows * sizeof(void *));
    for (size_t i = 0; i < rows; i++) {
        dynamic_2D[i] = realloc(dynamic_2D[i], row_sizes[i]);

        if (dynamic_2D[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(dynamic_2D[j]);
            }
            free(dynamic_2D);
            return NULL;
        }
    }

    return dynamic_2D;
}


int censor(const char *search, FILE *in, FILE *out)
{
    char *line;
    size_t idx = 0;
    size_t len_search = strlen(search);

    if (len_search == 0) {
        return INVALID_SEARCH;
    }

    while ((line = readline(in)) != NULL) {
        while (line[idx] != '\0') {
            if (strstr(&line[idx], search) == &line[idx]) {
                memset(&line[idx], '*', len_search);
                idx += len_search - 1;
            } else {
                idx++;
            }
        }
        fputs(line, out);
        free(line);
    }

    return 0;
}

int gcd(int a, int b) {
    int rem = 0;
    if (b > a) {
        int aux = a;
        a = b;
        b = aux;
    }
    while (b != 0) {
        rem = a;
        a = b;
        b = rem % b;
    }
    return a;
}


int compare_int(const void* a, const void* b) {
    return (*(int *) a - *(int *) b);
}

int compare_ch(const void* a, const void* b) {
    return (*(char *) a - *(char *) b);
}

int array[10] = {25, 89, 45, 1, 0, 47, 6, 34, 8, 10};
    char array2[7] = {'A', 'a', 'd', 'b', 'B', 'D', 'z'};

    qsort(array, 10, sizeof(int), compare_int);

    for (int i = 0; i < 10; i++) {
        printf("%d, ", array[i]);
    }
    printf("\n");


    qsort(array2, 7, sizeof(char), compare_ch);

    for (int j = 0; j < 7; j++) {
        printf("%c, ", array2[j]);
    }
    printf("\n");


int file_peek(FILE* file)
{
    int c = getc(file);
    return c == EOF ? EOF : ungetc(c, file);
}

while (file_peek(file) != EOF) {
    // create 2D array of lines and each time save the line there
    fscanf(file, "%s\n", line);
}
