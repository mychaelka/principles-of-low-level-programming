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
    int last_nonempty = last_op;
    //int last_op = 'P';
    while (current != ';' && current != '='){
        int64_t out = 0;

        if (!(last_nonempty >= '0' && last_nonempty <= '9')){
            if (!isspace(current) && !(current >= '0' && current <= '9')){
                print_error_message("SYNTAX ERROR");
                return acc;
            }
        }

        //if (!isspace(current)){
        //    last_nonempty = current;
        //}

        // reading numbers and whitespaces
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
            acc /= out;
        }
        else if (last_op == '%'){
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
    printf("last nonempty is %c\n", last_nonempty);
    printf("# %ld\n", acc);
    return acc;
}


bool calculate(void)
{
    int64_t acc = 0;
    int ch;
    while ((ch=getchar()) != EOF){ // 1 loop = loading of 1 character from stdin
        if (ch == 'N'){
            acc = 0;
            printf("# %ld\n", acc);
        }

        else if (ch == ';'){ // in case of ';' continue
            continue;
        }

        else if (ch == '='){ // in case of '=' print acc and continue
            printf("# %ld\n", acc);
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
