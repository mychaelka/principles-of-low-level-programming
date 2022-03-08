#include <stdbool.h>
#include "minesweeper.h"

#define UNUSED(A) (void) (A)

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return false;
}

bool is_mine(uint16_t cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return false;
}

bool is_revealed(uint16_t cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return false;
}

int get_number(uint16_t cell)
{
    // TODO: Implement me
    UNUSED(cell);
    return -1;
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{
    // TODO: Implement me
    UNUSED(cell);
    UNUSED(val);
    return false;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me

    // The postprocess function should be called at the end of the load_board function
    return postprocess(rows, cols, board);
}

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    return -1;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    return -1;
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
