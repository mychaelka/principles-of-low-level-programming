#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
}

void change_base();


int64_t read_command(long acc, int last_op) // acc = 0 at first
{
    int current = getchar();
    //int last_op = 'P';
    while (current != ';' && current != '='){
        int64_t out = 0;

        // reading numbers and whitespaces
        while ((current >= '0' && current <= '9') || isspace(current)) {
            int num = current - '0';

            if (!isspace(current)){
                out *= 10;
                out += num;
            }
            current = getchar();
        }
        if (last_op == 'P'){
            acc = out;
        }
        if (last_op == '+'){
            acc += out;
        }
        if (last_op == '-'){
            acc -= out;
        }
        if (last_op == '*'){
            acc *= out;
        }
        if (last_op == '/'){
            acc /= out;
        }
        if (last_op == '%'){
            acc %= out;
        }

        // when next character is an operator
        if (current == '+' || current == '-' || current == '*' || current == '/' || current == '%'){
            printf("# %ld\n", acc);
            last_op = current;
            current = getchar();
            continue;
        }
        if (current == '='){
            printf("# %ld\n", acc);
            break;
        }
        if (current == ';'){
            break;
        }

        current = getchar();
    }

    printf("# %ld\n", acc);
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

        if (ch == 'P'){
            acc = read_command(acc, ch);
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
