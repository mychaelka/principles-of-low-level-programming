#include "cut.h"

#include "../minesweeper.h"
#include "utils_for_tests.h"

/* Our provided tests are testing:
 *
 * set_cell - subset of valid states
 * postprocess - on simple valid board
 * assisting functions - is_mine, is_revealed ...
 * show_cell - subset of valid cells/states
 *
 * These functions are necessary for us, to test your representation.
 *
 * NOTE: These tests are not complete at all, they just give you a rough idea,
 *       if you are working correctly. They should also motivate you to create
 *       some of your own.
 *
 * Additional tests:
 * - print_board (this test is not a part of the nanecisto tests)
 * - load_board (this test is not a part of the nanecisto tests)
 *
 * We have provided you with these additional tests to give you a simple example
 * how the Output and Input should look like.
 *
 * As it was said previously, you should extend these provided tests
 * with your own. */

TEST(set_cell_with_mine_uppercase)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'M'));
    // THEN
    ASSERT(is_mine(dummy_cell));
}

TEST(set_cell_with_mine_lovercase)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'm'));
    // THEN
    ASSERT(is_mine(dummy_cell));
}

TEST(set_cell_revealed)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, '.'));
    // THEN
    ASSERT(is_revealed(dummy_cell));
}

TEST(postprocess_on_simple_board_3by3_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    fill_board_value(size, size, board, '.'); // function from test utils
    set_cell(&(board[1][1]), 'M');
    // THEN
    ASSERT(postprocess(size, size, board) == 1); // it has one mine
}

TEST(show_cell_on_3by3_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    const char *input = "XXX"
                        "XWF"
                        "XMX";
    // THEN
    fill_board(size, size, board, input); // function from test utils

    ASSERT(show_cell(board[0][0]) == 'X');
    ASSERT(show_cell(board[1][1]) == 'F'); // this is a wrong flag, but in print it should be shown as flag
    ASSERT(show_cell(board[1][2]) == 'F'); // this is a correct flag
    ASSERT(show_cell(board[2][1]) == 'X'); // this is mine, but it should not be revealed
    ASSERT(show_cell(board[2][2]) == 'X');
}

/* *** NOTE ***
 * This test it NOT part of the sanity (nanecisto) suite and will not
 * prevent you from submitting your solution.
 *
 * It is here only to show you the expected formatting of 'print_board()'. */
TEST(print_revealed_all_but_mine_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    const char *input = "..."
                        ".M."
                        "...";
    fill_board(size, size, board, input);

    // THEN
    ASSERT(print_board(size, size, board) == 0);

    ASSERT_FILE(stdout, "     0   1   2 \n"
                        "   +---+---+---+\n"
                        " 0 | 1 | 1 | 1 |\n"
                        "   +---+---+---+\n"
                        " 1 | 1 |XXX| 1 |\n"
                        "   +---+---+---+\n"
                        " 2 | 1 | 1 | 1 |\n"
                        "   +---+---+---+\n");
}

/* *** NOTE ***
 * This test it NOT part of the sanity (nanecisto) suite and will not
 * prevent you from submitting your solution.
 *
 * It is here only to show you the expected formatting of 'print_board()'. */
TEST(load_unrevealed_3by3_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("XXX"
                 "XMX"
                 "XXX"); // the content of STDIN for LOAD

    // THEN
    ASSERT(load_board(size, size, board) == 1); // it has one mine

    ASSERT(is_mine(board[1][1]));
    ASSERT(!is_mine(board[1][0]));
}

/* TODO: You should write your tests for specified other functions and values
 * in the similar manner. */

TEST(mine_in_corner)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("MXXXX"
                 "XMXXX"
                 "XXXXX");

    // THEN
    ASSERT(load_board(size, size, board) == -1);
}

TEST(zero_mines)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("XXXXX"
                 "XXXXX"
                 "XXXXX");

    // THEN
    ASSERT(load_board(size, size, board) == -1);
}

TEST(wrong_number_surrounding_mines)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("X2MXX"
                 "XMXXX"
                 "XXX1X");

    // THEN
    ASSERT(load_board(size, size, board) == -1);
}

TEST(correct_number_surrounding_mines)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("X2MXX"
                 "XMXXX"
                 "XXM1X"
                 "XXXXX"
                 "X...X");

    // THEN
    ASSERT(load_board(size, size, board) == 3);

    ASSERT(print_board(size, size, board) == 0);

    ASSERT_FILE(stdout, "     0   1   2   3   4 \n"
                        "   +---+---+---+---+---+\n"
                        " 0 |XXX| 2 |XXX|XXX|XXX|\n"
                        "   +---+---+---+---+---+\n"
                        " 1 |XXX|XXX|XXX|XXX|XXX|\n"
                        "   +---+---+---+---+---+\n"
                        " 2 |XXX|XXX|XXX| 1 |XXX|\n"
                        "   +---+---+---+---+---+\n"
                        " 3 |XXX|XXX|XXX|XXX|XXX|\n"
                        "   +---+---+---+---+---+\n"
                        " 4 |XXX|   |   |   |XXX|\n"
                        "   +---+---+---+---+---+\n");
}

TEST(all_mines_except_corners)
{
    // GIVEN
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    INPUT_STRING(".MM."
                 "MMMM"
                 "MMMM"
                 ".MM.");

    // THEN
    ASSERT(load_board(size, size, board) == 12);
    ASSERT(get_number(board[1][2]) == 0);
    ASSERT(print_board(size, size, board) == 0);

    ASSERT_FILE(stdout, "     0   1   2   3 \n"
                        "   +---+---+---+---+\n"
                        " 0 | 3 |XXX|XXX| 3 |\n"
                        "   +---+---+---+---+\n"
                        " 1 |XXX|XXX|XXX|XXX|\n"
                        "   +---+---+---+---+\n"
                        " 2 |XXX|XXX|XXX|XXX|\n"
                        "   +---+---+---+---+\n"
                        " 3 | 3 |XXX|XXX| 3 |\n"
                        "   +---+---+---+---+\n");
}

TEST(set_cell_flagged)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'f'));
    // THEN
    ASSERT(is_flag(dummy_cell));
}

TEST(set_cell_number)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, '8'));
    // THEN
    ASSERT(is_revealed(dummy_cell));
}

TEST(set_cell_wrong_number)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(!set_cell(&dummy_cell, '9'));
}

TEST(postprocess_on_5x5_board)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    fill_board_value(size, size, board, '.'); // function from test utils
    set_cell(&(board[1][1]), 'M');
    set_cell(&(board[2][1]), 'M');
    set_cell(&(board[1][4]), 'M');
    set_cell(&(board[4][2]), 'M');
    set_cell(&(board[1][3]), 'M');
    // THEN
    ASSERT(postprocess(size, size, board) == 5);
}

TEST(find_mines_1)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxxxx000xx221xxxx1xxxxx");
    // THEN
    ASSERT(find_mines(size, size, board) == 2);
}

TEST(find_mines_fail)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxxxx000xxx21xxxxxxxxxx");
    // THEN
    ASSERT(find_mines(size, size, board) == -1);
}


TEST(find_mines_irregular_shape_fail)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[7][5] = { 0 };
    INPUT_STRING("xxxxxxx000xxx21xxxxxxxxxxx111xx1x1x");
    // THEN
    ASSERT(find_mines(7, size, board) == -1);
}

TEST(find_mines_irregular_shape_success)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[7][5] = { 0 };
    INPUT_STRING("xxxxxxx000xxx21xxxxxx000xx111xx1x1x");
    // THEN
    ASSERT(find_mines(7, size, board) == 1);
}

TEST(find_mines_corner)
{
    // GIVEN
    const size_t size = 5;
    uint16_t board[7][5] = { 0 };
    INPUT_STRING("xxxxxxx000xxx21xxxxxx0000x1111xx01x");
    // THEN
    ASSERT(find_mines(7, size, board) == 2);
}
//...xxxxxxx...xxxxxxx.wwwwwxxxx.wwwwwwxxx.wwwwwxxxx...xxxxxxx...xxxxxxm...xxxxxxx...xx...xx...xx...xx




/* If you've made it down here I would also would like to tell you the meaning
 * of life is 42. */
