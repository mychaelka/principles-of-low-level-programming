#include <stdio.h>
#include <stdlib.h>

void fill_matrix(int min_value, int max_value, size_t mrows, size_t mcols, int matrix[mrows][mcols]);
void print_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols]);
void multiply_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols], int number);
int find_max_min_in_array(size_t length, int *array, int *max, int *min);

int main(void)
{
    int matrix[5][5];
    fill_matrix(1, 10, 5, 5, matrix);
    int size = sizeof(matrix);
    int size1 = sizeof(matrix[0]);
    int size2 = sizeof(matrix[1]);
    int size3 = sizeof(matrix[1][1]);
    printf("matrix is pointing to %p with size of %zu\n", matrix, size);
    printf("matrix[0] is pointing to %p with size of %zu\n", matrix[0], size1);
    printf("matrix[1] is pointing to %p with size of %zu\n", matrix[1], size2);
    printf("matrix[1][1] is pointing to %p with size of %zu\n", matrix[1][1], size3);

    print_matrix(5, 5, matrix);
    printf("\n");
    printf("\n");
    multiply_matrix(5, 5, matrix, 3);
    print_matrix(5, 5, matrix);
    return 0;
}

void multiply_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols], int number)
{
    for (size_t i = 0; i < mrows; i++) {
        for (size_t j = 0; j < mcols; j++) {
            matrix[i][j] *= number;
        }
    }
}

int* matrix_to_array(size_t mrows, size_t mcols, int matrix[mrows][mcols])
{
    size_t length = mrows * mcols;
    int array[length];
    int k = 0;
    for (size_t i=0; i<mrows; i++) {
        for (size_t j=0; j<mcols; j++) {
            array[k] = matrix[i][j];
            k++;
        }
    }
    return array; // nerobit vo funkcii
}


void fill_matrix(int min_value, int max_value, size_t mrows, size_t mcols, int matrix[mrows][mcols])
{
    if (max_value <= min_value) {
        return;
    }

    for (size_t i = 0; i < mrows; i++) {
        for (size_t j = 0; j < mcols; j++) {
            matrix[i][j] = rand() % (max_value - min_value) + min_value;
        }
    }
}

void print_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols])
{
    for (size_t i = 0; i < mrows; i++) {
        for (size_t j = 0; j < mcols; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}
