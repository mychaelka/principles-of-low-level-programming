#include <stdio.h>
#include <stdlib.h>

int find_max_in_array(int array[10]) {
    int max = array[0];
    for (int i=0; i<10; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}


int find_min_in_array(int array[10]) {
    int min = array[0];
    for (int i=0; i<10; i++) {
        if (array[i] < min) {
            min = array[i];
        }
    }
    return min;
}

int find_max_min_in_array(size_t length, int *array, int *max, int *min) {
    int rv = 2;
    if (max == NULL && min == NULL) {
        return 0;
    }
    if (max == NULL){
        *max = array[0];
    }
    if (min == NULL){
        *min = array[0];
    }

    for (int i=0; i<10; i++) {
        if (max == NULL && min != NULL) {
            rv = 1;
            if (array[i] < *min) {
                *min = array[i];
            }
        }
        if (max != NULL && min == NULL) {
            rv = 1;
            if (array[i] > *max) {
                *max = array[i];
            }
        }
        if (max != NULL && min != NULL) {
            if (array[i] < *min) {
                *min = array[i];
            }
            if (array[i] > *max) {
                *max = array[i];
            }
        }
    }
    return rv;
}

int find_sum_in_array(size_t length, int *array) {
    int sum = 0;
    for (int i=0; i<length; i++) {
        sum += array[i];
    }
    return sum;
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

int find_gcd_in_array(size_t length, int *array) {
    int divisor = gcd(array[0], array[1]);
    for (int i=1; i < length; i++) {
        divisor = gcd(divisor, array[i]);
    }
    return divisor;
}

int main(void)
{
    int ch;
    int arr[10];
    int *arr_ptr = arr;
    int current = 0;
    int i = 0;

    printf("Zadejte 10 cislic: ");
    while ((ch=getchar()) != EOF) {
        if (!(ch >= '0' && ch <= '9')) {
            arr[i] = current;
            current = 0;
            i++;
        }
        else {
            int num = ch - 48;
            current *= 10;
            current += num;
        }

    }

    printf("Array: [");
    for (i=0; i<9; i++) {
        printf("%d, ", arr[i]);
    }
    printf("%d]\n", arr[9]);

    int sum = find_sum_in_array(10, arr_ptr);
    printf("Sum: %d\n", sum);

    //int max = find_max_in_array(arr);
    //int min = find_min_in_array(arr);
    int max = 0;
    int *max_ptr = &max;
    int min = 0;
    int *min_ptr = &min;
    find_max_min_in_array(10, arr_ptr, max_ptr, min_ptr);
    int divisor = find_gcd_in_array(10, arr_ptr);
    printf("Max: %d; ", max);
    printf("Min: %d\n", min);
    printf("GCD: %d; ", divisor);
    printf("Factors: [");
    for (i=0; i<9; i++) {
        printf("%d, ", arr[i]/divisor);
    }
    printf("%d]\n", arr[9]/divisor);

    return EXIT_SUCCESS;
}

// 33 22 121 110 77 264 396 187 913 462