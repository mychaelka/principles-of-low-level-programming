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
    if (cell >= 100 || (cell >= 40 && cell <= 48)) {
        return true;
    }
    return false;
}

bool is_revealed(uint16_t cell)
{
    if ((cell >= 0 && cell <= 8) || (cell >= 20 && cell <= 28)) {
        return true;
    }
    return false;
}

int get_number(uint16_t cell)
{
    if (is_mine(cell)) {
        return 0;
    }
    else {
        return cell % 10;
    }
}

size_t set_start(size_t index)
{
    size_t start = 0;

    if (index != 0) {
        start = index - 1;
    }
    return start;
}

size_t set_stop(size_t index, size_t length)
{
    size_t stop = length;

    if (index != length) {
        stop = index + 1;
    }
    return stop;
}

int count_board_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int mines = 0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (is_mine(board[i][j])) {
                mines += 1;
            }
        }
    }
    return mines;
}

int count_board_flags(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int flags = 0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (is_flag(board[i][j])) {
                flags += 1;
            }
        }
    }
    return flags;
}

int count_unrevealed(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int unrevealed = 0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (!is_revealed(board[i][j])) {
                unrevealed += 1;
            }
        }
    }
    return unrevealed;
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{
    int mines = *cell % 10;
    if (val == 'F' || val == 'f') {
        *cell = 40 + mines; // if F, then the mine flag is correct
        return true;
    }
    if (val == 'W' || val == 'w') {
        *cell = 30 + mines;
        return true;
    }
    if (val == 'X' || val == 'x') {
        *cell = 10 + mines;
        return true;
    }
    if (val >= '0' && val <= '8') {
        *cell = val - 48;
        return true;
    }
    if (val == 'M' || val == 'm') {
        *cell = 100 + mines;
        return true;
    }
    if (val == '.') {
        *cell = 20 + mines;
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

        if (!set_cell(&board[current_row][current_col], current)) {  // invalid character
            continue;
        }
        loaded += 1;

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

            //if (board[i][j] >= 0 && board[i][j] <= 8) {  // create helper function count_mines
            size_t start_x = set_start(i);
            size_t end_x = set_stop(i, rows);
            size_t start_y = set_start(j);
            size_t end_y = set_stop(j, cols);
            int surrounding_mines = 0;

            for (size_t x = start_x; x <= end_x; x++) {
                for (size_t y = start_y; y <= end_y; y++) {
                    if (!(x == i && y == j) && is_mine(board[x][y])) {
                        surrounding_mines += 1;
                    }
                }
            }

            if ((board[i][j] >= 0 && board[i][j] <= 8)) {
                if ((surrounding_mines != board[i][j])) {
                    return -1;
                }
            }
            else {
                board[i][j] += surrounding_mines;
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
            if (row >= 10) {
                printf("%zu ", i / 2 - 1);
            } else {
                printf(" %zu ", i / 2 - 1);
            }
        } else {
            printf("   ");
        }
        for (size_t j = 0; j < cols; j++) {
            if (i == 0) {
                if (j >= 10) {
                    printf(" %zu ", j);
                } else {
                    printf("  %zu ", j);
                }
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

                if (board[row][j] >= 30 && board[row][j] <= 48) {
                    printf("|_F_");
                }
                else if (board[row][j] == 1000) {  // uncovered mine
                    printf("| M ");
                }
                else if (board[row][j] == 0) {
                    printf("|   ");
                }
                else if ((board[row][j] > 0 && board[row][j] <= 8) || board[row][j] == 20) {
                    printf("| %d ", board[row][j]);
                }
                else if (board[row][j] > 20 && board[row][j] <= 28) {
                    printf("| %d ", board[row][j] - 20);
                }
                else {
                    printf("|XXX");
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
    if (cell >= 100) {
        return 'X';
    }
    if (cell == 0 || cell == 20) {
        return ' ';
    }
    if ((cell >= 30 && cell <= 48)) {
        return 'F';
    }
    if (cell > 0 && cell <= 8){
        return (char) cell;
    }
    if (cell > 20 && cell <= 28) {
        return (char) (cell - 20);
    }
    //(cell >= 10 && cell <= 18) {

    return 'X';
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (board[row][col] >= 100) {
        reveal_single(&board[row][col]);
        print_board(rows, cols, board);
        return 1;
    }
    if (board[row][col] >= 30 && board[row][col] <= 48) {
        return -1;
    }
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return -1;
    }
    if ((board[row][col] >= 0 && board[row][col] <= 8) || (board[row][col] >= 20 && board[row][col] <= 28)) {
        return -1;
    }
    if (board[row][col] > 10 && board[row][col] <= 18) {
        reveal_single(&board[row][col]);
        print_board(rows, cols, board);
        return 0;
    }

    return 0;
}

int reveal_single(uint16_t *cell)
{
    if (*cell >= 100) {
        *cell = 1000;
        return 1;
    }
    if (*cell >= 30 && *cell <= 48) {
        return -1;
    }
    if (*cell >= 10 && *cell <= 18) {
        *cell -= 10;
        return 0;
    }


    return 0;
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
    int cell = board[row][col];
    if (cell >= 100) {
        set_cell(&board[row][col], 'F');
    }
    else if (cell >= 10 && cell <= 18) {
        set_cell(&board[row][col], 'W');
    }
    else if (cell >= 30 && cell <= 48) {
        //int mines = cell % 10;
        set_cell(&board[row][col], 'X');
        //board[row][col] += mines;
    }
    return count_board_mines(rows, cols, board) - count_board_flags(rows, cols, board);
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (count_unrevealed(rows, cols, board) == count_board_mines(rows, cols, board)) {
        return true;
    }
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
