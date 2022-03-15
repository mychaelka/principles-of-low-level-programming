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

    char result[64];
    int i, idx = 0;
    while (num > 0) {
        rem = num % 2;

        rem += 48;

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

uint64_t rotate(uint64_t accum, unsigned int bits, bool left)
{
    bits %= 64;
    if (bits == 0) {
        return accum;
    }
    if (left) {
        return ((accum << bits) | (accum >> (64 - bits)));
    }
    return ((accum >> bits) | (accum << (64 - bits)));
}

bool is_arithmetic_op(int current)
{
    return (current == '+' || current == '-' || current == '*' || current == '/' ||
            current == '%' || current == '<' || current == '>' || current == 'P' ||
            current == 'l' || current == 'r');
}

uint64_t read_number(int current, bool *return_val_ptr)
{
    uint64_t out = 0;
    uint64_t num = current - '0';

    if (isspace(current)) {
        return out;
    }

    if (toupper(current) >= 'A') {
        out = 10 + toupper(current) - 'A';
    } else {
        out = num;
    }

    if (out > UINT64_MAX - num) { // loaded number would overflow
        print_error_message("Out of range");
        *return_val_ptr = false;
        return out;
    }

    return out;
}

bool is_allowed_digit(int base, int current) // whitespaces are allowed while reading number
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

uint64_t arithmetic_operation(int operator, uint64_t out, bool * return_val_ptr, uint64_t accum)
{
    switch (operator) {
    case 'P':
        accum = out;
        break;
    case '+':
        if (out > UINT64_MAX - accum) {
            print_error_message("Out of range");
            *return_val_ptr = false;
            return accum;
        }
        accum += out;
        break;
    case '-':
        if (out > accum) {
            print_error_message("Out of range");
            *return_val_ptr = false;
            return accum;
        }
        accum -= out;
        break;
    case '*':
        if (out != 0 && UINT64_MAX / out < accum) {
            print_error_message("Out of range");
            *return_val_ptr = false;
            return accum;
        }
        accum *= out;
        break;
    case '/':
        if (out == 0) {
            print_error_message("Division by zero");
            *return_val_ptr = false;
            return accum;
        }
        accum /= out;
        break;
    case '%':
        if (out == 0) {
            print_error_message("Division by zero");
            *return_val_ptr = false;
            return accum;
        }
        accum %= out;
        break;
    case '<':
        if (out > 63) {
            print_error_message("Out of range");
            *return_val_ptr = false;
            return accum;
        }
        accum <<= out;
        break;
    case '>':
        if (out > 63) {
            print_error_message("Out of range");
            *return_val_ptr = false;
            return accum;
        }
        accum >>= out;
        break;
    case 'l':
        accum = rotate(accum, out, true);
        break;
    case 'r':
        accum = rotate(accum, out, false);
        break;
    default:
        break;
    }
    return accum;
}

uint64_t read_command(uint64_t accum, int last_op, uint64_t *memory_ptr, bool *return_val_ptr)
{
    int current = getchar();
    int base = 10;
    bool non_digit = true; // if true, last read character was not a digit or whitespace
    uint64_t out = 0;

    while (true) {
        if (base != 10) { // after base switch, an allowed number must follow
            if (!is_allowed_digit(base, current)) {
                print_error_message("Syntax error");
                *return_val_ptr = false;
                return accum;
            }
        }

        if (non_digit && current != 'T' && current != 'X' && current != 'O' &&
                !isspace(current) && current != 'N' && current != 'M' &&
                current != 'm') { // after non_digit was read, only allowed
                                  // characters are digits/base switches/memory chars
            if (!is_allowed_digit(base, current)) {
                print_error_message("Syntax error");
                *return_val_ptr = false;
                return accum;
            }
            while (is_allowed_digit(base, current)) {
                if (!isspace(current)) {
                    if (UINT64_MAX / base < out) {
                        print_error_message("Out of range");
                        *return_val_ptr = false;
                        return accum;
                    }
                    out *= base;
                    out += read_number(current, return_val_ptr);
                    non_digit = false;
                }
                current = getchar();
            }
            base = 10; // after reading next number, base switches to default
            accum = arithmetic_operation(last_op, out, return_val_ptr, accum);
            if (!*return_val_ptr) {
                return accum;
            }
            printf("# %lu\n", accum);
            out = 0;
        }

        switch (current) {
        case '=':
            printf("# %lu\n", accum);
            return accum;
        case ';':
            while (current != '\n') {
                if (current == EOF) {
                    break;
                }
                current = getchar();
            }
            return accum;
        case EOF:
            return accum;
        case 'N':
            accum = 0;
            printf("# %lu\n", accum);
            return accum;

        // operations with memory
        case 'M':
            *memory_ptr += accum;
            return accum;
        case 'R':
            *memory_ptr = 0;
            return accum;
        case 'm':
            if (!non_digit) {
                print_error_message("Syntax error");
                *return_val_ptr = false;
                return accum;
            }
            out = *memory_ptr;
            accum = arithmetic_operation(last_op, out, return_val_ptr, accum);
            if (!*return_val_ptr) {
                return accum;
            }
            printf("# %lu\n", accum);
            return accum;

        // switching base with argument
        case 'X':
            base = 16;
            current = getchar();
            printf("# %lX\n", accum);
            if (!non_digit) {
                return accum;
            }
            non_digit = true;
            continue;
        case 'O':
            base = 8;
            current = getchar();
            printf("# %lo\n", accum);
            if (!non_digit) {
                return accum;
            }
            non_digit = true;
            continue;
        case 'T':
            base = 2;
            current = getchar();
            print_binary(accum);
            if (!non_digit) {
                return accum;
            }
            non_digit = true;
            continue;
        default:
            break;
        }

        if (isspace(current)) {
            current = getchar();
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
        *return_val_ptr = false;
        return accum;
    }
}

int main(void)
{
    uint64_t accum = 0; // accumulator
    uint64_t memory = 0;
    uint64_t *memory_ptr = &memory;
    bool return_val = true;
    bool *return_val_ptr = &return_val;

    int ch;
    while ((ch = getchar()) != EOF) {
        if (isspace(ch)) {
            continue;
        } else if (ch == ';') {
            while (ch != '\n') {
                if (ch == EOF) {
                    break;
                }
                ch = getchar();
            }
        } else if (ch == '=') {
            printf("# %lu\n", accum);
        } else if (ch == 'N') {
            accum = 0;
            printf("# %lu\n", accum);
        } else if (ch == 'M') {
            memory += accum;
        } else if (ch == 'R') {
            memory = 0;
        }

        // base switchers (when standing without argument)
        else if (ch == 'X') {
            printf("# %lX\n", accum);
            continue;
        } else if (ch == 'O') {
            printf("# %lo\n", accum);
            continue;
        } else if (ch == 'T') {
            print_binary(accum);
            continue;
        }

        // arithmetic operations and bit rotations
        else if (is_arithmetic_op(ch)) {
            accum = read_command(accum, ch, memory_ptr, return_val_ptr);
            if (!return_val) { // error occurred while reading next command
                return EXIT_FAILURE;
            }
        } else {
            print_error_message("Syntax error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
