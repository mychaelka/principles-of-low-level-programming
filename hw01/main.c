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
    bool non_digit = true;
    while (true){//current != ';' && current != '='){
        unsigned long long int out = 0;

        //printf("last nonempty: %c\n", last_nonempty);
        //printf("current: %c\n", current);

        if (isspace(current)){  // ignore whitespaces
            current = getchar();
            continue;
        }

        if (current == EOF){
            printf("# %llu\n", acc);
            exit(0);
        }

        // reading numbers with whitespaces
        while ((current >= '0' && current <= '9') || isspace(current)) {
            int num = current - '0';

            if (!isspace(current)){
                non_digit = false;
                out *= 10;
                out += num;
            }
            current = getchar();
        }

        if (non_digit){
            print_error_message("SYNTAX ERROR");
            exit(1);
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
        else if (last_op == '<'){
            acc <<= out;
        }
        else if (last_op == '>'){
            acc >>= out;
        }

        if (current == 'N'){
            printf("# %llu\n", acc);
            acc = 0;
            printf("# %llu\n", acc);
            //current = getchar();
            return acc;
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

        // when next character is an operator
        if (current == '+' || current == '-' || current == '*' || current == '/'
            || current == '%' || current == '<' || current == '>' || current == 'P'){
            printf("# %llu\n", acc);
            last_op = current;
            non_digit = true;
            current = getchar();
            continue;
        }

        current = getchar();
        if (!isspace(current)){
            non_digit = true;
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

        else if (ch == 'P' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%'
                 || ch == '<' || ch == '>'){
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
