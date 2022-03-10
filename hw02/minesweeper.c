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

int set_start(int index)
{
    int start = 0;

    if (index != 0) {
        start = index - 1;
    }
    return start;
}

int set_stop(int index, int length)
{
    int stop = length;

    if (index != length) {
        stop = index + 1;
    }
    return stop;
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{
    if (val == 'F' || val == 'f') {
        *cell = 20; // if F, then the mine flag is correct
        return true;
    }
    if (val == 'W' || val == 'w') {
        *cell = 30;
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
    size_t current_row = 0;
    size_t current_col = 0;
    size_t loaded = 0;
    while (loaded != rows*cols) {
        int ch = getchar();
        char current = (char) ch;   // loads only row*col chars

        if (set_cell(&board[current_row][current_col], current)) {
            loaded += 1;
        }

        if (current_col == (cols - 1)) {
           current_row += 1;
           current_col = 0;
        }
        else {
            current_col += 1;
        }
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

            if (board[i][j] >= 0 && board[i][j] <= 8) {  // create helper function count_mines
                int start_x = set_start(i);
                int end_x = set_stop(i, rows);
                int start_y = set_start(j);
                int end_y = set_stop(j, cols);
                int surrounding_mines = 0;

                for (int x = start_x; x <= end_x; x++) {
                    for (int y = start_y; y <= end_y; y++) {
                        if (x != y && is_mine(board[x][y])) {
                            surrounding_mines += 1;
                            //board[i][j] += 1;
                        }
                    }
                }

                if ((board[i][j] >= 0 && board[i][j] <= 8) && (surrounding_mines != board[i][j])) {
                    return -1;
                }
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
    int row = 0;
    for (size_t i = 0; i <= 2*rows + 1; i++) {
        if (i % 2 == 0 && i > 0) {
            printf(" %zu ", i / 2 - 1);
        } else {
            printf("   ");
        }
        for (size_t j = 0; j < cols; j++) {
            if (i == 0) {
                printf("  %zu ", j);
                if (j == cols - 1) {
                    printf("\n");
                }
            }
            else if (i % 2 != 0) {
                printf("+---");
                if (j == cols - 1) {
                    printf("+\n");
                }
            }
            else if (i % 2 == 0) {
                if (board[row][j] == 100) {
                    printf("|XXX");
                }
                else if (board[row][j] == 20 || board[row][j] == 30) {
                    printf("|_F_");
                }
                else if (board[row][j] == 10) {
                    printf("|XXX");
                }
                else if (board[row][j] == 0) {
                    printf("|   ");
                }
                else if (board[row][j] > 0 && board[row][j] <= 8) {
                    printf("| %d ", board[row][j]);
                }
                if (j == cols - 1) {
                    printf("|\n");
                    row += 1;
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
