#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "minesweeper.h"

#define UNUSED(A) (void) (A)

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */


bool is_flag(uint16_t cell)
{
    if (cell >= 30 && cell <= 48) {
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
    size_t stop = length - 1;

    if (index != length - 1) {
        stop = index + 1;
    }
    return stop;
}

/**
 * @brief Function to count occurrences of different types of cells (mines, flags, unrevealed)
 *
 * Exactly one bool parameter from (mines, flags, unrevealed) must be set to true,
 * all others should be set to false!
 *
 * @param rows Number of rows
 * @param cols Number of cols
 * @param board 2D array that represents the mines board
 * @param mines if true, returns number of mines in board
 * @param flags if true, returns number of flags in board
 * @param unrevealed if true, returns number of unrevealed cells in board
 * @return number of occurrences of specified cell, otherwise -1 (any error)
 */
int count_cells(size_t rows, size_t cols, uint16_t board[rows][cols], bool mines, bool flags, bool unrevealed)
{
    int mines_count = 0;
    int flags_count = 0;
    int unrevealed_count = 0;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (is_mine(board[i][j])) {
                mines_count += 1;
            }
            if (is_flag(board[i][j])) {
                flags_count += 1;
            }
            if (!is_revealed(board[i][j])) {
                unrevealed_count += 1;
            }
        }
    }
    if (mines) {
        return mines_count;
    }
    if (flags) {
        return flags_count;
    }
    if (unrevealed) {
        return unrevealed_count;
    }

    return -1;
}


void print_board_character(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (board[row][col] >= 30 && board[row][col] <= 48) {
        printf("|_F_");
    }
    else if (board[row][col] == 1000) {  // uncovered mine
        printf("| M ");
    }
    else if (board[row][col] == 0 || board[row][col] == 20) {
        printf("|   ");
    }
    else if ((board[row][col] > 0 && board[row][col] <= 8)) {
        printf("| %d ", board[row][col]);
    }
    else if (board[row][col] > 20 && board[row][col] <= 28) {
        printf("| %d ", board[row][col] - 20);
    }
    else {
        printf("|XXX");
    }
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{
    // mine = 100
    // wrong flag = 30 -- 38
    // correct flag = 40 -- 48
    // covered field = 10 -- 18
    // uncovered field = 0 -- 8
    // uncovered field without known mines = 20 -- 28

    int mines = *cell % 10;

    switch (toupper(val)) {
        case 'F':
            *cell = 40 + mines;
            return true;
        case 'W':
            *cell = 30 + mines;
            return true;
        case 'X':
            *cell = 10 + mines;
            return true;
        case 'M':
            *cell = 100 + mines;
            return true;
        case '.':
            *cell = 20 + mines;
            return true;
        default:
            break;
    }

    if (val >= '0' && val <= '8') {
        *cell = val - 48;
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

        if (ch == EOF) {
            return -1;
        }

        char current = (char) ch;

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

            // create helper function count_mines
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

int postprocess_reduced(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (rows < MIN_SIZE || cols < MIN_SIZE || rows > MAX_SIZE || cols > MAX_SIZE) {
        return -1;
    }

    return 0;
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
                print_board_character(rows, cols, board, row, j);

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

    return 'X';
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (board[row][col] >= 100) {
        reveal_single(&board[row][col]);
        return 1;
    }
    if ((board[row][col] >= 30 && board[row][col] <= 48)       // flag
        || (row < 0 || row >= rows || col < 0 || col >= cols)  // wrong dimension
        || (board[row][col] >= 0 && board[row][col] <= 8))      // already revealed cell
        //|| (board[row][col] >= 20 && board[row][col] <= 28))   // already revealed cell -- can this actually occur?
    {
        return -1;
    }

    if (board[row][col] == 10) {
        reveal_floodfill(rows, cols, board, row, col);
    }
    else if (board[row][col] > 10 && board[row][col] <= 18) {
        reveal_single(&board[row][col]);
    }

    return 0;
}

int reveal_single(uint16_t *cell)
{
    if (*cell >= 100) {
        *cell = 1000; // change to revealed mine and lose
        return 1;
    }
    if (is_flag(*cell)) {
        *cell -= (*cell / 10) * 10; // changes its value to 0-8 (revealed cell)
        return 0;                   // happens only in floodfill (wrong flag)!
    }
    if (*cell >= 10 && *cell <= 18) {
        *cell -= 10;
        return 0;
    }

    return 0;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if ((board[row][col] > 10 && board[row][col] <= 18)) {
        reveal_single(&board[row][col]);
    }
    if (board[row][col] == 10 || is_flag(board[row][col])) {
        reveal_single(&board[row][col]);
        size_t start_x = set_start(row);
        size_t end_x = set_stop(row, rows);
        size_t start_y = set_start(col);
        size_t end_y = set_stop(col, cols);

        for (size_t x = start_x; x <= end_x; x++) {
            for (size_t y = start_y; y <= end_y; y++) {
                if (!(x == row && y == col)) {
                    reveal_floodfill(rows, cols, board, x, y);
                }
            }
        }
    }
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int cell = board[row][col];
    if (cell >= 0 && cell <= 8) {
        return INT16_MIN;
    }
    if (cell >= 100) { // this is mine
        set_cell(&board[row][col], 'F');
    }
    else if (cell >= 10 && cell <= 18) { // not mine
        set_cell(&board[row][col], 'W');
    }
    else if (cell >= 30 && cell <= 48) { // reverse already placed flag (doesn't matter if wrong)
        set_cell(&board[row][col], 'X');
    }
    return count_cells(rows, cols, board, true, false, false)
           - count_cells(rows, cols, board, false, true, false);
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (count_cells(rows, cols, board, false, false, true)
        == count_cells(rows, cols, board, true, false, false)) {
        return true;
    }
    return false;
}

/* ************************************************************** *
 *                         BONUS FUNCTIONS                        *
 * ************************************************************** */

int generate_random_index(size_t limit)
{
    int num = rand() % limit;
    return num;
}

int generate_random_board(size_t rows, size_t cols, uint16_t board[rows][cols], size_t mines)
{
    size_t set_mines = 0;

    if (mines < 0 || mines > 96) { // mines can't be in corners, that's why maximum possible is 96
        return -1;
    }

    while (set_mines < mines) {
        int row = generate_random_index(rows); // randomly generate mine indices
        int col = generate_random_index(cols);

        if (is_mine(board[row][col])) {
            continue;
        }
        if (row % (rows - 1) == 0 && col % (cols - 1) == 0) { // refuse corner indices
            continue;
        }
        set_cell(&board[row][col], 'M');
        set_mines += 1;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (!is_mine(board[i][j])) {
                set_cell(&board[i][j], 'X');
            }
        }
    }

    // The postprocess function should be called at the end of the generate random board function
    return postprocess(rows, cols, board);
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols]) // re-do this function
{
    size_t current_row = 0;
    size_t current_col = 0;
    size_t loaded = 0;
    while (loaded != rows*cols) {
        int ch = getchar();

        if (ch == EOF) {
            return -1;
        }

        char current = (char) ch;

        if (current == 'X' || (current >= '0' && current <= '8')) {
            set_cell(&board[current_row][current_col], current);
            loaded += 1;
        }
        else {
            continue;
        }

        if (current_col == (cols - 1)) {
            current_row += 1;
            current_col = 0;
        }
        else {
            current_col += 1;
        }
    }

    if (postprocess_reduced(rows, cols, board) < 0) {
        return -1;
    }


    int flagged_mines = 0;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {

            if (is_revealed(board[i][j])) {
                int surrounding_unrevealed = 0;

                size_t start_x = set_start(i);
                size_t end_x = set_stop(i, rows);
                size_t start_y = set_start(j);
                size_t end_y = set_stop(j, cols);

                for (size_t x = start_x; x <= end_x; x++) {
                    for (size_t y = start_y; y <= end_y; y++) {
                        if ((!(x == i && y == j)) && (is_flag(board[x][y]) || !(is_revealed(board[x][y])))) {
                            surrounding_unrevealed += 1;
                        }
                    }
                }

                if (surrounding_unrevealed == get_number(board[i][j])) {
                    for (size_t x = start_x; x <= end_x; x++) {
                        for (size_t y = start_y; y <= end_y; y++) {
                            if ((!(x == i && y == j)) && !(is_revealed(board[x][y])) && !(is_flag(board[x][y]))) {
                                set_cell(&board[x][y], 'F');
                                flagged_mines += 1;
                            }
                        }
                    }
                }
            }
        }
    }
    if (flagged_mines == 0) {
        return -1;
    }

    return flagged_mines;
}
