#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
}


unsigned long long int read_command(unsigned long long int acc, int last_op) // acc = 0 at first
{
    int current = getchar();
    int last_nonempty = last_op;
    while (current != ';' && current != '='){
        unsigned long long int out = 0;

        if (isspace(current)){  // ignore whitespaces
            current = getchar();
            continue;
        }

        if (current == EOF){
            printf("# %llu\n", acc);
            exit(0);
        }

        if (!(last_nonempty >= '0' && last_nonempty <= '9')){
            if (!(isspace(current) || (current >= '0' && current <= '9'))){
                print_error_message("SYNTAX ERROR");
                exit(1);
            }
        }

        // reading numbers with whitespaces
        while ((current >= '0' && current <= '9') || isspace(current)) {
            int num = current - '0';

            if (!isspace(current)){
                last_nonempty = current;
                out *= 10;
                out += num;
            }
            current = getchar();
        }

        if (last_op == 'P'){
            acc = out;
        }
        else if (last_op == '+'){
            acc += out;
        }
        else if (last_op == '-'){
            acc -= out;
        }
        else if (last_op == '*'){
            acc *= out;
        }
        else if (last_op == '/'){
            if (out == 0){
                print_error_message("DIVISION BY ZERO");
                exit(1);
            }
            acc /= out;
        }
        else if (last_op == '%'){
            acc %= out;
        }

        // when next character is an operator
        if (current == '+' || current == '-' || current == '*' || current == '/' || current == '%'){
            printf("# %llu\n", acc);
            last_op = current;
            last_nonempty = current;
            current = getchar();
            continue;
        }
        if (current == '='){
            printf("# %llu\n", acc);
            break;
        }
        if (current == ';'){
            while (current != '\n') {
                current = getchar();
            }
            break;
        }

        current = getchar();
        if (!isspace(current)) {
            last_nonempty = current;
        }

    }

    printf("# %llu\n", acc);
    return acc;
}


bool calculate(void)
{
    unsigned long long int acc = 0;
    int ch;
    while ((ch=getchar()) != EOF){ // 1 loop = loading of 1 character from stdin
        if (ch == 'N'){
            acc = 0;
            printf("# %llu\n", acc);
        }

        else if (ch == ';'){ // in case of ';' continue
            while (ch != '\n') {
                ch = getchar();
            }
            continue;
        }

        else if (ch == '='){ // in case of '=' print acc and continue
            printf("# %llu\n", acc);
        }

        else if (ch == 'P' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%'){
            acc = read_command(acc, ch);
        }

        else if (isspace(ch)){
            continue;
        }

        else {
            print_error_message("SYNTAX ERROR");
            return false;
        }
    }

    return true;
}

int main(void)
{
    if (!calculate()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
