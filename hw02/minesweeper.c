#include "minesweeper.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell)
{
    return (cell >= 30 && cell <= 48);
}

bool is_mine(uint16_t cell)
{
    return (cell >= 100 || (cell >= 40 && cell <= 48));
}

bool is_revealed(uint16_t cell)
{
    return (cell <= 8 || (cell >= 20 && cell <= 28) || cell == 1000);
}

int get_number(uint16_t cell)
{
    if (is_mine(cell)) {
        return 0;
    }
    return cell % 10;
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
 * @brief Function to count surrounding mines or unrevealed fields of a specific
 * cell
 *
 * Exactly one bool parameter from (mines, unrevealed) must be set to true,
 * the other must be set to false!
 *
 * @param rows Number of rows
 * @param cols Number of cols
 * @param board 2D array that represents the mines board
 * @param mines if true, function returns number of mines in board, otherwise
 * returns number of unrevealed cells in board
 * @return number of occurrences of either mines or unrevealed cells
 */
int count_surrounding(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col, bool mines)
{
    int surrounding_mines = 0;
    int surrounding_unrevealed = 0;

    for (size_t x = set_start(row); x <= set_stop(row, rows); x++) {
        for (size_t y = set_start(col); y <= set_stop(col, cols); y++) {
            if ((x != row || y != col) && is_mine(board[x][y])) {
                surrounding_mines += 1;
            }
            if ((x != row || y != col) &&
                    (is_flag(board[x][y]) || !(is_revealed(board[x][y])))) {
                surrounding_unrevealed += 1;
            }
        }
    }
    if (mines) {
        return surrounding_mines;
    }
    return surrounding_unrevealed;
}

int flag_surrounding_cells(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int flagged_mines = 0;

    for (size_t x = set_start(row); x <= set_stop(row, rows); x++) {
        for (size_t y = set_start(col); y <= set_stop(col, cols); y++) {
            if ((x != row || y != col) && (!(is_revealed(board[x][y]))) &&
                    !(is_flag(board[x][y]))) {
                set_cell(&board[x][y], 'F');
                flagged_mines += 1;
            }
        }
    }

    return flagged_mines;
}

int check_board_size(size_t rows, size_t cols)
{
    if (rows < MIN_SIZE || cols < MIN_SIZE || rows > MAX_SIZE ||
            cols > MAX_SIZE) {
        return -1;
    }

    return 0;
}

bool cell_outside_board(size_t rows, size_t cols, size_t row, size_t col)
{
    return (row >= rows || col >= cols);
}

/**
 * @brief Function to count occurrences of different types of cells (mines,
 * flags, unrevealed) in board
 *
 * Exactly one bool parameter from (mines, flags, unrevealed) must be set to
 * true, all others must be set to false!
 *
 * @param rows Number of rows
 * @param cols Number of cols
 * @param board 2D array that represents the mines board
 * @param mines if true, function returns number of mines in board
 * @param flags if true, function returns number of flags in board
 * @param unrevealed if true, function returns number of unrevealed cells in
 * board
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

int count_board_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    return count_cells(rows, cols, board, true, false, false);
}

int count_board_flags(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    return count_cells(rows, cols, board, false, true, false);
}

int count_board_unrevealed(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    return count_cells(rows, cols, board, false, false, true);
}

void print_board_character(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (board[row][col] >= 30 && board[row][col] <= 48) {
        printf("|_F_");
    } else if (board[row][col] == 1000) { // uncovered mine
        printf("| M ");
    } else if (board[row][col] == 0 || board[row][col] == 20) {
        printf("|   ");
    } else if ((board[row][col] > 0 && board[row][col] <= 8)) {
        printf("| %d ", board[row][col]);
    } else if (board[row][col] > 20 && board[row][col] <= 28) {
        printf("| %d ", board[row][col] - 20);
    } else {
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

    if (cell == NULL) {
        return false;
    }

    *cell = 0;

    switch (toupper(val)) {
    case 'F':
        *cell = 40;
        return true;
    case 'W':
        *cell = 30;
        return true;
    case 'X':
        *cell = 10;
        return true;
    case 'M':
        *cell = 100;
        return true;
    case '.':
        *cell = 20;
        return true;
    default:
        break;
    }

    if (val >= '0' && val <= '8') {
        *cell = val - '0';
        return true;
    }

    return false;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (check_board_size(rows, cols) < 0) {
        return -1;
    }

    size_t current_row = 0;
    size_t current_col = 0;
    size_t loaded = 0;
    while (loaded != rows * cols) {
        int ch = getchar();

        if (ch == EOF) {
            return -1;
        }

        char current = (char) ch;

        if (!set_cell(&board[current_row][current_col],
                    current)) { // invalid character
            continue;
        }
        loaded += 1;

        if (current_col == (cols - 1)) {
            current_row += 1;
            current_col = 0;
        } else {
            current_col += 1;
        }
    }

    // The postprocess function should be called at the end of the load_board
    // function
    return postprocess(rows, cols, board);
}

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int mines = 0;
    int surrounding_mines;

    if (check_board_size(rows, cols) < 0) {
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

            surrounding_mines = count_surrounding(rows, cols, board, i, j, true);

            if (board[i][j] <= 8) {
                if ((surrounding_mines != board[i][j])) {
                    return -1;
                }
            } else {
                board[i][j] = (board[i][j] / 10) * 10 + surrounding_mines;
            }
        }
    }
    if (mines == 0) {
        return -1;
    }
    return mines;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

void print_row_separator(size_t cols)
{
    printf("   ");
    for (size_t j = 0; j < cols; j++) {
        printf("+---");
        if (j == cols - 1) {
            printf("+\n");
        }
    }
}

void print_col_numbers(size_t cols)
{
    printf("   ");
    for (size_t j = 0; j < cols; j++) {
        printf("%3zu ", j);
        if (j == cols - 1) {
            putchar('\n');
        }
    }
}

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int row = 0;
    print_col_numbers(cols);

    for (size_t row_number = 1; row_number <= rows; row_number++) {
        print_row_separator(cols);
        printf("%2zu ", row_number - 1);

        for (size_t col = 0; col < cols; col++) {
            print_board_character(rows, cols, board, row, col);
        }
        printf("|\n");
        row += 1;
    }
    print_row_separator(cols);
    return 0;
}

char show_cell(uint16_t cell)
{
    if (cell == 1000) {
        return 'M';
    }
    if (cell >= 100) {
        return 'X';
    }
    if (cell == 0 || cell == 20) {
        return ' ';
    }
    if ((cell >= 30 && cell <= 48)) {
        return 'F';
    }
    if (is_revealed(cell)) {
        return (char) (get_number(cell) + '0');
    }

    return 'X';
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (cell_outside_board(rows, cols, row, col)) {
        return -1;
    }
    if (is_revealed(board[row][col]) || is_flag(board[row][col])) {
        return -1;
    }
    if (board[row][col] >= 100) {
        reveal_single(&board[row][col]);
        return 1;
    }
    if (board[row][col] == 10) {
        reveal_floodfill(rows, cols, board, row, col);
    } else if (board[row][col] > 10 && board[row][col] <= 18) {
        reveal_single(&board[row][col]);
    }

    return 0;
}

int reveal_single(uint16_t *cell)
{
    if (cell == NULL) {
        return -1;
    }

    if (*cell >= 100) {
        *cell = 1000; // change to revealed mine and lose
        return 1;
    }
    if (is_flag(*cell) || is_revealed(*cell)) {
        return -1;
    }
    if (*cell >= 10 && *cell <= 18) {
        *cell -= 10;
        return 0;
    }

    return -1;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (cell_outside_board(rows, cols, row, col)) {
        return;
    }

    if ((board[row][col] > 10 && board[row][col] <= 18)) {
        reveal_single(&board[row][col]);
    }
    if (board[row][col] == 10 ||
            (board[row][col] >= 30 && board[row][col] <= 38)) {
        if (is_flag(board[row][col])) {
            flag_cell(rows, cols, board, row, col);
        }
        reveal_single(&board[row][col]);

        for (size_t x = set_start(row); x <= set_stop(row, rows); x++) {
            for (size_t y = set_start(col); y <= set_stop(col, cols); y++) {
                if (!(x == row && y == col)) {
                    reveal_floodfill(rows, cols, board, x, y);
                }
            }
        }
    }
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (cell_outside_board(rows, cols, row, col)) {
        return INT16_MIN;
    }

    uint16_t cell = board[row][col];
    if (cell <= 8) {
        return INT16_MIN;
    }
    if (cell >= 100) {
        set_cell(&board[row][col], 'F');
    } else if (cell >= 10 && cell <= 18) { // not mine
        set_cell(&board[row][col], 'W');
    } else if (cell >= 30 && cell <= 38) { // reverse wrong flag
        set_cell(&board[row][col], 'X');
    } else if (cell >= 40 && cell <= 48) {
        set_cell(&board[row][col], 'M'); // reverse correct flag
    }
    return count_board_mines(rows, cols, board) -
            count_board_flags(rows, cols, board);
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    return count_board_unrevealed(rows, cols, board) ==
            count_board_mines(rows, cols, board);
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
    if (check_board_size(rows, cols) < 0) {
        return -1;
    }

    if (mines > rows * cols - 4 || mines == 0) { // mines can't be in corners
        return -1;
    }

    size_t set_mines = 0;

    while (set_mines < mines) {
        int row = generate_random_index(rows); // randomly generate mine indices
        int col = generate_random_index(cols);

        if (is_mine(board[row][col])) {
            continue;
        }
        if (row % (rows - 1) == 0 &&
                col % (cols - 1) == 0) { // refuse corner indices
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

    // The postprocess function should be called at the end of the generate random
    // board function
    return postprocess(rows, cols, board);
}

int load_board_for_finding_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (check_board_size(rows, cols) < 0) {
        return -1;
    }

    size_t current_row = 0;
    size_t current_col = 0;
    size_t loaded = 0;
    while (loaded != rows * cols) {
        int ch = getchar();

        if (ch == EOF) {
            return -1;
        }

        char current = (char) ch;

        if (toupper(current) == 'X' || (current >= '0' && current <= '8')) {
            set_cell(&board[current_row][current_col], current);
            loaded += 1;
        } else {
            continue;
        }

        if (current_col == (cols - 1)) {
            current_row += 1;
            current_col = 0;
        } else {
            current_col += 1;
        }
    }

    return 0;
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (load_board_for_finding_mines(rows, cols, board) < 0) {
        return -1;
    }

    int flagged_mines = 0;
    int surrounding_unrevealed;

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            if (is_revealed(board[row][col])) {
                surrounding_unrevealed =
                        count_surrounding(rows, cols, board, row, col, false);

                if (surrounding_unrevealed == get_number(board[row][col])) {
                    flagged_mines += flag_surrounding_cells(rows, cols, board, row, col);
                }
            }
        }
    }
    if (flagged_mines == 0) {
        return -1;
    }

    return flagged_mines;
}
