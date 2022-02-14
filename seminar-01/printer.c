#include <stdio.h>

int main(void)
{
    int number = 12;
    long long long_number = 0x27BFE;
    double floating_number = 2.71828182;
    char single_char = 'A';
    char string[] = "This is string";

    /* Zde definujte formatovaci retezce */
    printf("Number is: %d\n", number);
    printf("Long number is: %llx in hexadecimal\n", long_number);
    printf("Long number is %lld in decimal\n", long_number);
    printf("Floating number is: %lf\n", floating_number);
    printf("Floating number is: %.4lf with precision to 4 decimal places\n", floating_number);
    printf("Char is: '%c'\n", single_char);
    printf("String is: \"%s\"\n", string);

    return 0;
}
