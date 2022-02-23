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

void read()
{
    int64_t acc = 0;
    int ch;
    while ((ch=getchar()) != EOF){ // 1 loop = loading of 1 character from stdin
        if (ch == ';'){ // in case of ';' continue
            continue;
        }

        if (ch == '='){ // in case of '=' print acc and continue
            printf("# %ld\n", acc);
        }

        if (ch == 'P'){
            acc = 0;
            ch = getchar();
            while ((ch >= '0' && ch <= '9') || isspace(ch)) {
                int num = ch - '0';
                if (!isspace(ch)){ // this works for now - leave it be
                    acc *= 10;
                    acc += num;
                }
                ch = getchar();
            }
            if (ch == ';' || ch == '='){ // in case of '=' print acc and continue
                printf("# %ld\n", acc);
            }
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
