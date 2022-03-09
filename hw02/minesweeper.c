#include <stdbool.h>
#include <stdio.h>
#include "minesweeper.h"

#define UNUSED(A) (void) (A)

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell)
{
    if (cell >= 20 && cell <= 28) {
        return true;
    }
    return false;
}

bool is_mine(uint16_t cell)
{
    if (cell == 100 || cell == 1000) {
        return true;
    }
    return false;
}

bool is_revealed(uint16_t cell)
{
    if ((cell >= 0 && cell <= 8) || cell == 1000) {
        return true;
    }
    return false;
}

int get_number(uint16_t cell)
{
    if (is_mine(cell)) {
        return 0;
    }
    return -1;
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{
    if (val == 'F' || val == 'f') {
        *cell = 20; // TODO: count surrounding mines
        return true;
    }
    if (val == 'W' || val == 'w') {
        *cell = 20;
        return true;
    }
    if (val == 'X' || val == 'x') {
        *cell = 10;
        return true;
    }
    if (val >= '0' && val <= '8') {
        *cell = val - 48;
        return true;
    }
    if (val == 'M' || val == 'm') {
        *cell = 100;
        return true;
    }
    if (val == '.') {
        *cell = 0;
        return true;
    }
    return false;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int ch = getchar();
    int current_row = 0;
    int current_col = 0;
    while (ch != EOF) {
        char current = (char) ch;

        if ((current_col + 1) % cols == 0) {
           current_row += 1;
           current_col = 0;
        }

        set_cell(&board[current_row][current_col], current);

        current_col += 1;
        ch = getchar();
    }

    // The postprocess function should be called at the end of the load_board function
    return postprocess(rows, cols, board);
}

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int mines = 0;
    if (rows < MIN_SIZE || cols < MIN_SIZE || rows > MAX_SIZE || cols > MAX_SIZE) {
        return -1;
    }
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            // mine in corners
            if (i % (rows - 1) == 0 && j % (cols - 1) == 0) {
                if (is_mine(board[i][j])) {
                    return -1;
                }
            }
            // mine found
            if (is_mine(board[i][j])) {
                mines += 1;
            }
        }
    }
    if (mines == 0) {
        return -1 ;
    }
    return mines;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    for (size_t i = 0; i <= 2*rows + 1; i++) {
        if (i % 2 == 0 && i > 0) {
            printf(" %zu ", i / 2);
        } else {
            printf("   ");
        }
        for (size_t j = 1; j <= cols; j++) {
            if (i == 0) {
                printf("  %zu ", j);
                if (j == cols) {
                    printf("\n");
                }
            }
            else if (i % 2 != 0) {
                printf("+---");
                if (j == cols) {
                    printf("+\n");
                }
            }
            else if (i % 2 == 0) {
                printf("|   ");
                if (j == cols) {
                    printf("|\n");
                }
            }
        }
    }
    return 0;
}

char show_cell(uint16_t cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return -1;
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    UNUSED(row);
    UNUSED(col);
    return -1;
}

int reveal_single(uint16_t *cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return -1;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    UNUSED(row);
    UNUSED(col);
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    UNUSED(row);
    UNUSED(col);
    return -1;
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    return false;
}

/* ************************************************************** *
 *                         BONUS FUNCTIONS                        *
 * ************************************************************** */

int generate_random_board(size_t rows, size_t cols, uint16_t board[rows][cols], size_t mines)
{
    // TODO: Implement me
    UNUSED(mines);

    // The postprocess function should be called at the end of the generate random board function
    return postprocess(rows, cols, board);
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    return -1;
}
