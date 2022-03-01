#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_error_message(char *message)
{
    fprintf(stderr, "%s\n", message);
}

void print_base(uint64_t num, int base)
{
    uint64_t rem;

    if (num == 0) {
        printf("# 0\n");
        return;
    }

    char result[10000];
    int i, idx = 0;
    while (num > 0) {
        rem = num % base;
        if (rem < 10) {
            rem += 48;
        } else {
            rem += 55;
        }
        result[idx] = rem;

        num /= base;
        idx++;
    }
    printf("# ");
    for (i = idx - 1; i >= 0; i--) {
        printf("%c", result[i]);
    }
    printf("\n");
}

uint64_t read_command(uint64_t acc, int last_op, uint64_t *mem, bool *rv) // acc = 0 at first
{
    int current = getchar();
    bool non_digit = true;
    bool hex_switcher = false;
    bool okt_switcher = false;
    bool bin_switcher = false;
    while (true) {
        uint64_t out = 0;

        if (isspace(current)) { // ignore whitespaces
            current = getchar();
            continue;
        }

        if (current == '=') {
            printf("# %lu\n", acc);
            break;
        }
        if (current == ';') {
            while (current != '\n') {
                current = getchar();
            }
            break;
        }
        if (current == EOF) {
            printf("# %lu\n", acc);
            *rv = true;
            return acc;
        }

        if (current == 'X') {                                   // now this will expect a number
            if (hex_switcher || okt_switcher || bin_switcher) { // switcher already in place
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            hex_switcher = true;
            print_base(acc, 16);
            current = getchar();
            continue;
        } else if (current == 'O') {
            if (hex_switcher || okt_switcher || bin_switcher) {
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            okt_switcher = true;
            print_base(acc, 8);
            current = getchar();
            continue;
        } else if (current == 'T') {
            if (hex_switcher || okt_switcher || bin_switcher) {
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            bin_switcher = true;
            print_base(acc, 2);
            current = getchar();
            continue;
        }

        if (hex_switcher) { // base switchers
            if (!isxdigit(current)) {
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            while (isxdigit(current) || isspace(current)) {
                uint64_t num = current - '0';

                if (!isspace(current)) {
                    if (num > UINT64_MAX - out) {
                        print_error_message("Out of range");
                        *rv = false;
                        return acc;
                    }
                    hex_switcher = false;
                    non_digit = false;
                    if (current == 'A' || current == 'a') {
                        out *= 16;
                        out += 10;
                    } else if (current == 'B' || current == 'b') {
                        out *= 16;
                        out += 11;
                    } else if (current == 'C' || current == 'c') {
                        out *= 16;
                        out += 12;
                    } else if (current == 'D' || current == 'd') {
                        out *= 16;
                        out += 13;
                    } else if (current == 'E' || current == 'e') {
                        out *= 16;
                        out += 14;
                    } else if (current == 'F' || current == 'f') {
                        out *= 16;
                        out += 15;
                    } else {
                        out *= 16;
                        out += num;
                    }
                }
                current = getchar();
            }
        } else if (okt_switcher) {
            if (!(current >= '0' && current <= '7')) {
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            while ((current >= '0' && current <= '7') || isspace(current)) {
                uint64_t num = current - '0';

                if (!isspace(current)) {
                    if (num > UINT64_MAX - out) {
                        print_error_message("Out of range");
                        *rv = false;
                        return acc;
                    }
                    okt_switcher = false;
                    non_digit = false;
                    out *= 8;
                    out += num;
                }
                current = getchar();
            }
        } else if (bin_switcher) {
            if (!(current == '0' || current == '1')) {
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            while (current == '0' || current == '1' || isspace(current)) {
                uint64_t num = current - 48;

                if (!isspace(current)) {
                    if (num > UINT64_MAX - out) {
                        print_error_message("Out of range");
                        *rv = false;
                        return acc;
                    }
                    bin_switcher = false;
                    non_digit = false;
                    out *= 2;
                    out += num;
                }
                current = getchar();
            }
        } else {
            // reading numbers with whitespaces
            while ((current >= '0' && current <= '9') || isspace(current)) {
                uint64_t num = current - '0';

                if (!isspace(current)) {
                    if (num > UINT64_MAX - out) {
                        print_error_message("Out of range");
                        *rv = false;
                        return acc;
                    }
                    non_digit = false;
                    out *= 10;
                    out += num;
                }
                current = getchar();
            }
        }

        if (non_digit) {
            if (current != 'X' && current != 'O' && current != 'T' && current != 'm') {
                printf("# %lu\n", acc);
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
        }

        if (last_op == 'P') { // P can be followed only with one m
            if ((non_digit && current != 'm') || ((!non_digit) && current == 'm')) {
                acc = out;
                printf("# %lu\n", acc);
                print_error_message("Syntax error");
                *rv = false;
                return acc;
            }
            if (current == 'm') {
                acc += *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                acc = out;
            }
        } else if (last_op == '+') {
            if (current == 'm') {
                acc += *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                if (out > UINT64_MAX - acc) {
                    print_error_message("Out of range");
                    *rv = false;
                    return acc;
                } else {
                    acc += out;
                }
            }
        } else if (last_op == '-') {
            if (current == 'm') {
                acc -= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                if (out > acc) {
                    print_error_message("Out of range");
                    *rv = false;
                    return acc;
                } else {
                    acc -= out;
                }
            }
        } else if (last_op == '*') {
            if (current == 'm') {
                acc *= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                if (UINT64_MAX / out < acc) {
                    print_error_message("Out of range");
                    *rv = false;
                    return acc;
                } else {
                    acc *= out;
                }
            }
        } else if (last_op == '/') {
            if (current == 'm') {
                if (*mem == 0) {
                    print_error_message("Division by zero");
                    *rv = false;
                    return acc;
                }
                acc /= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                if (out == 0) {
                    print_error_message("Division by zero");
                    *rv = false;
                    return acc;
                }
                acc /= out;
            }
        } else if (last_op == '%') {
            if (current == 'm') {
                acc %= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                acc %= out;
            }
        } else if (last_op == '<') {
            if (current == 'm') {
                acc <<= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                acc <<= out;
            }
        } else if (last_op == '>') {
            if (current == 'm') {
                acc >>= *mem;
                current = getchar();
                non_digit = false;
                continue;
            } else {
                acc >>= out;
            }
        }

        if (current == 'N') {
            printf("# %lu\n", acc);
            acc = 0;
            printf("# %lu\n", acc);
            return acc;
        }
        if (current == 'M') {
            *mem = acc; // change pointer - but this possibly can't happen
            current = getchar();
            continue;
        }
        if (current == 'R') {
            *mem = 0;
            current = getchar();
            continue;
        }

        if (current == '=') {
            printf("# %lu\n", acc);
            break;
        }
        if (current == ';') {
            while (current != '\n') {
                current = getchar();
            }
            break;
        }
        if (current == EOF) {
            printf("# %lu\n", acc);
            *rv = true;
            return acc;
        }

        // when current character is an operator, set last_op to its value and continue
        if (current == '+' || current == '-' || current == '*' || current == '/' || current == '%' || current == '<' || current == '>' || current == 'P') {
            printf("# %lu\n", acc);
            last_op = current;
            non_digit = true;
            current = getchar();
            continue;
        } else {
            printf("# %lu\n", acc);
            print_error_message("Syntax error");
            *rv = false;
            return acc;
        }
    }

    printf("# %lu\n", acc);
    return acc;
}

bool calculate(void)
{
    unsigned long int acc = 0;
    uint64_t memory = 0;
    uint64_t *mem;
    mem = &memory;
    bool return_val = true;
    bool *rv = &return_val;
    int ch;
    while ((ch = getchar()) != EOF) { // 1 loop = loading of 1 character from stdin
        if (ch == 'N') {
            acc = 0;
            printf("# %lu\n", acc);
        }

        else if (ch == 'M') {
            *mem += acc;
        }

        else if (ch == 'R') {
            *mem = 0;
        }

        else if (ch == ';') { // in case of ';' continue
            while (ch != '\n') {
                ch = getchar();
            }
            continue;
        }

        else if (ch == '=') { // in case of '=' print acc and continue
            printf("# %lu\n", acc);
        }

        else if (ch == 'P' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '<' || ch == '>') {
            acc = read_command(acc, ch, mem, rv);
            if (!return_val) {
                return false;
            }
        }

        else if (ch == 'X') {
            print_base(acc, 16);
            continue;
        } else if (ch == 'O') {
            print_base(acc, 8);
            continue;
        } else if (ch == 'T') {
            print_base(acc, 2);
            continue;
        }

        else if (isspace(ch)) {
            continue;
        }

        else {
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
