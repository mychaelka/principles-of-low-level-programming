#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
}

void print_binary(uint64_t num)
{
    uint64_t rem;

    if (num == 0) {
        printf("# 0\n");
        return;
    }

    char result[10000];
    int i, idx = 0;
    while (num > 0) {
        rem = num % 2;
        if (rem < 10) {
            rem += 48;
        } else {
            rem += 55;
        }
        result[idx] = rem;
        num /= 2;
        idx++;
    }
    printf("# ");
    for (i = idx - 1; i >= 0; i--) {
        printf("%c", result[i]);
    }
    printf("\n");
}

uint64_t rotate(uint64_t **acc, unsigned int bits, bool left)
{
    bits %= 64;
    if (bits == 0) {
        return **acc;
    }
    if (left) {
        return ((**acc << bits) | (**acc >> (512 - bits))); // 64 * 8
    }
    return ((**acc >> bits) | (**acc << (512 - bits)));
}

bool is_arithmetic_op(int current)
{
    if (current == '+' || current == '-' || current == '*' || current == '/' ||
            current == '%' || current == '<' || current == '>' || current == 'P' ||
            current == 'l' || current == 'r') {
        return true;
    }
    return false;
}

uint64_t read_number(int current, bool *rv)
{
    uint64_t out = 0;
    uint64_t num = current - '0';

    if (isspace(current)) {
        return out;
    }
    if (current == 'A' || current == 'a') {
        out = 10;
    } else if (current == 'B' || current == 'b') {
        out = 11;
    } else if (current == 'C' || current == 'c') {
        out = 12;
    } else if (current == 'D' || current == 'd') {
        out = 13;
    } else if (current == 'E' || current == 'e') {
        out = 14;
    } else if (current == 'F' || current == 'f') {
        out = 15;
    } else {
        out = num;
    }

    if (out > UINT64_MAX - num) { // loaded number would overflow
        print_error_message("Out of range");
        *rv = false;
        return out;
    }

    return out;
}

bool is_allowed_digit(int base, int current)
{
    if (base == 10) {
        if ((current >= '0' && current <= '9') || isspace(current)) {
            return true;
        }
    } else if (base == 16) {
        if (isxdigit(current) || isspace(current)) {
            return true;
        }
    } else if (base == 8) {
        if ((current >= '0' && current <= '7') || isspace(current)) {
            return true;
        }
    } else if (base == 2) {
        if (current == '0' || current == '1' || isspace(current)) {
            return true;
        }
    }
    return false;
}

void arithmetic_operation(int operator, uint64_t out, bool *rv, uint64_t **acc)
{
    if (operator== 'P') {
        **acc = out;
    } else if (operator== '+') {
        if (out > UINT64_MAX - **acc) {
            print_error_message("Out of range");
            *rv = false;
            return;
        }
        **acc += out;
    } else if (operator== '-') {
        if (out > **acc) {
            print_error_message("Out of range");
            *rv = false;
            return;
        }
        **acc -= out;
    } else if (operator== '*') {
        if (out != 0 && UINT64_MAX / out < **acc) {
            print_error_message("Out of range");
            *rv = false;
            return;
        }
        **acc *= out;
    } else if (operator== '/') {
        if (out == 0) {
            print_error_message("Division by zero");
            *rv = false;
            return;
        }
        **acc /= out;
    } else if (operator== '%') {
        if (out == 0) {
            print_error_message("Division by zero");
            *rv = false;
            return;
        }
        **acc %= out;
    } else if (operator== '<') {
        if (out > 63) {
            print_error_message("Out of range");
            *rv = false;
            return;
        }
        **acc <<= out;
    } else if (operator== '>') {
        if (out > 63) {
            print_error_message("Out of range");
            *rv = false;
            return;
        }
        **acc >>= out;
    } else if (operator== 'l') {
        **acc = rotate(acc, out, true);
        return;
    } else if (operator== 'r') {
        **acc = rotate(acc, out, false);
        return;
    }
}

uint64_t read_command(uint64_t *acc, int last_op, uint64_t *mem, bool *rv)
{
    int current = getchar();
    int base = 10;
    bool non_digit = true;
    uint64_t out = 0;
    uint64_t **acc_pt = &acc;

    while (true) {
        if (base != 10) { // after switch, a number must follow
            if (!is_allowed_digit(base, current)) {
                print_error_message("Syntax error");
                *rv = false;
                return *acc;
            }
        }

        if (non_digit && current != 'T' && current != 'X' && current != 'O' &&
                !isspace(current) && current != 'N' && current != 'M' &&
                current != 'm') { // if turned on, a number or base switch must follow
            if (!is_allowed_digit(base, current)) {
                print_error_message("Syntax error");
                *rv = false;
                return *acc;
            }
            while (is_allowed_digit(base, current)) {
                if (!isspace(current)) {
                    if (UINT64_MAX / base < out) {
                        print_error_message("Out of range");
                        *rv = false;
                        return *acc;
                    }
                    out *= base;
                    out += read_number(current, rv);
                    non_digit = false; // digits were successfully read
                }
                current = getchar();
            }
            base = 10; // after reading next number, base switches back to 10
            arithmetic_operation(last_op, out, rv, acc_pt);
            if (!*rv) { // arithmetic operation was not successful
                return *acc;
            }
            printf("# %lu\n", *acc);
            out = 0;
        }

        if (isspace(current)) { // ignore whitespaces
            current = getchar();
            continue;
        }
        if (current == '=') { // =,;,EOF -> end current command
            printf("# %lu\n", *acc);
            return *acc;
        }
        if (current == ';') {
            while (current != '\n') {
                if (current == EOF) {
                    break;
                }
                current = getchar();
            }
            return *acc;
        }
        if (current == EOF) {
            return *acc;
        }
        if (current == 'N') { // reset the accumulator
            *acc = 0;
            printf("# %lu\n", *acc);
            return *acc;
        }

        // operations with memory
        if (current == 'M') {
            *mem += *acc;
            return *acc;
        }
        if (current == 'R') {
            *mem = 0;
            return *acc;
        }
        if (current == 'm') {
            if (!non_digit) {
                print_error_message("Syntax error");
                *rv = false;
                return *acc;
            }
            out = *mem;
            arithmetic_operation(last_op, out, rv, acc_pt);
            if (!*rv) { // arithmetic operation was not successful
                return *acc;
            }
            printf("# %lu\n", *acc);
            return *acc;
        }

        // switching base with argument
        if (current == 'X') {
            base = 16;
            current = getchar();
            printf("# %lX\n", *acc);
            if (!non_digit) {
                return *acc;
            }
            non_digit = true;
            continue;
        }
        if (current == 'O') {
            base = 8;
            current = getchar();
            printf("# %lo\n", *acc);
            if (!non_digit) {
                return *acc;
            }
            non_digit = true;
            continue;
        } else if (current == 'T') {
            base = 2;
            current = getchar();
            print_binary(*acc);
            if (!non_digit) {
                return *acc;
            }
            non_digit = true;
            continue;
        }

        // when current character is an operator, set last_op to its value and
        // continue
        if (is_arithmetic_op(current)) {
            last_op = current;
            non_digit = true;
            current = getchar();
            continue;
        }

        // no other legal options
        print_error_message("Syntax error");
        *rv = false;
        return *acc;
    }
}

bool calculate()
{
    uint64_t accum = 0; // accumulator
    uint64_t *acc = &accum;
    uint64_t memory = 0;
    uint64_t *mem = &memory;
    bool return_val = true;
    bool *rv = &return_val;

    int ch = ' '; // characters that can begin a line: whitespace, ; = N M R P,
                  // arithmetic ops, X O T,
    while ((ch = getchar()) !=
            EOF) {         // main loop that goes either until EOF or until an error occurs
        if (isspace(ch)) { // skipping whitespaces
            continue;
        } else if (ch == ';') { // everything after ; and before \n is a comment
            while (ch != '\n') {
                if (ch == EOF) {
                    break;
                }
                ch = getchar();
            }
            continue;           // redundant??
        } else if (ch == '=') { // in case of '=' print accumulator and continue
            printf("# %lu\n", *acc);
        } else if (ch == 'N') {
            *acc = 0;
            printf("# %lu\n", *acc);
        } else if (ch == 'M') { // add accumulator to memory
            memory += *acc;
        } else if (ch == 'R') { // set memory to zero
            memory = 0;
        }

        // base switchers (when standing without argument)
        else if (ch == 'X') {
            printf("# %lX\n", *acc);
            continue;
        } else if (ch == 'O') {
            printf("# %lo\n", *acc);
            continue;
        } else if (ch == 'T') {
            print_binary(*acc);
            continue;
        }

        // arithmetic operations and bit rotations
        else if (ch == 'P' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
                ch == '%' || ch == '<' || ch == '>' || ch == 'l' || ch == 'r') {
            accum = read_command(acc, ch, mem, rv);
            if (!return_val) { // error occurred while reading next command
                return false;
            }
        } else {
            print_error_message("Syntax error");
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
