#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
}

void print_result();
void change_base();


int64_t read_next_number(long acc, int read)
{
    int64_t out = 0;
    int digit = getchar();
    while ((digit >= '0' && digit <= '9') || isspace(digit)) {
        int num = digit - '0';

        if (!isspace(digit)){ // this works for now - leave it be
            out *= 10;
            out += num;
        }
        digit = getchar();
    }

    if (read == '+'){
        acc += out;
        printf("# %ld\n", acc);
    }

    if (read == '*'){
        acc *= out;
        printf("# %ld\n", acc);
    }

    if (read == '-'){
        acc -= out;
        printf("# %ld\n", acc);
    }

    if (read == 'P'){
        acc = out;
        printf("# %ld\n", acc);
    }

    if (digit == '=' || digit == ';') {
        printf("# %ld\n", acc);
    }
    return acc;
}


void read()
{
    int64_t acc = 0;
    int ch;
    while ((ch=getchar()) != EOF){ // 1 loop = loading of 1 character from stdin
        if (ch == 'N'){
            acc = 0;
            printf("# %ld\n", acc);
        }

        if (ch == ';'){ // in case of ';' continue
            continue;
        }

        if (ch == '='){ // in case of '=' print acc and continue
            printf("# %ld\n", acc);
        }

        if (ch == 'P' || ch == '+' || ch == '*' || ch == '-'){
            acc = read_next_number(acc,ch);
        }
    }
}

bool calculate(void)
{
    read();

    print_error_message("SYNTAX ERROR");

    return false;
}

int main(void)
{
    if (!calculate()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
