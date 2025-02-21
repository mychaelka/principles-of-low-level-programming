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

TEST(reveal_cell_mine_and_others)
{
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("xmxxxxxxx");
    // THEN
    ASSERT(load_board(size, size, board) == 1);
    ASSERT(reveal_cell(size, size, board, 0, 1) == 1);
    ASSERT(reveal_cell(size, size, board, 0, 0) == 0);
}

TEST(reveal_cell_invalid_cases)
{
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("xmxfxxww.");
    // THEN
    ASSERT(load_board(size, size, board) == 2);
    ASSERT(reveal_cell(size, size, board, 2, 1) == -1);
    ASSERT(reveal_cell(size, size, board, 1, 0) == -1);
    ASSERT(reveal_cell(size, size, board, 0, 1) == 1);
    ASSERT(reveal_cell(size, size, board, 4, 1) == -1);
    ASSERT(reveal_cell(size, size, board, 2, 2) == -1);
    ASSERT(reveal_cell(size, size, board, 2, 1) == -1);
    ASSERT(is_revealed(board[0][1]));
}

TEST(reveal_cell_invalid_cases2)
{
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    INPUT_STRING("x22x xmmx xxxx xx.x");
    ASSERT(load_board(size, size, board) == 2);
    ASSERT(reveal_cell(size, size, board, 3, 2) == -1);
}


TEST(reveal_cell_only_mines) {
    const size_t size = 3;
    uint16_t board[3][3] = {0};
    // THEN
    fill_board_value(size, size, board, 'x');
    set_cell(&(board[1][2]), 'm');
    set_cell(&(board[1][1]), 'm');
    set_cell(&(board[2][1]), 'm');
    ASSERT(reveal_cell(size, size, board, 0, 0) == 0);
    ASSERT(reveal_cell(size, size, board, 1, 1) == 1);
    ASSERT(is_revealed(board[1][1]));
    ASSERT(is_revealed(board[0][0]));
    set_cell(&(board[2][1]), 'f');
    ASSERT(!is_revealed(board[2][1]));
    ASSERT(reveal_cell(size, size, board, 2, 1) == -1);
    ASSERT(!is_revealed(board[2][1]));
    set_cell(&(board[2][0]), '.');
    ASSERT(is_revealed(board[2][0]));
    ASSERT(reveal_cell(size, size, board, 2, 0) == -1);
    ASSERT(is_revealed(board[2][0]));
}

TEST(reveal_cell_floodfill) {
    const size_t size = 5;
    uint16_t board[5][5] = {0};
    INPUT_STRING("xxxxx xxxxx xmxxx xxmxx xxxxx");
    ASSERT(load_board(size, size, board) == 2);
    ASSERT(reveal_cell(size, size, board, 0, 0) == 0);
    ASSERT(is_revealed(board[1][1]));
    ASSERT(is_revealed(board[0][0]));
    ASSERT(!is_revealed(board[2][0]));
    ASSERT(is_revealed(board[2][2]));
    ASSERT(!is_revealed(board[2][1]));
    ASSERT(!is_revealed(board[3][2]));
}

TEST(reveal_cell_floodfill_flags) {
    const size_t size = 5;
    uint16_t board[5][5] = {0};
    INPUT_STRING("xxwxx xxxxx xmxxx xxmxx xxxfx");
    ASSERT(load_board(size, size, board) == 3);
    ASSERT(reveal_cell(size, size, board, 0, 0) == 0);
    ASSERT(is_revealed(board[1][1]));
    ASSERT(is_revealed(board[0][2]));
    ASSERT(!is_revealed(board[2][0]));
    ASSERT(is_revealed(board[2][2]));
    ASSERT(!is_revealed(board[2][1]));
    ASSERT(!is_revealed(board[3][2]));
}

TEST(reveal_cell_mines_and_flags)
{
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("xww wwf wmw");
    //ASSERT(reveal_single() == -1);
    ASSERT(load_board(size, size, board) == 2);
    ASSERT(reveal_cell(size, size, board, 0, 0) == 0);
    ASSERT(is_revealed(board[1][0]));
    ASSERT(is_revealed(board[1][1]));
    ASSERT(is_revealed(board[0][1]));
    ASSERT(!is_revealed(board[1][2]));
}

TEST(reveal_single_invalid_cases)
{
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("xmxfxxww.");
    // THEN
    ASSERT(load_board(size, size, board) == 2);
    //ASSERT(reveal_single() == -1);
    ASSERT(!is_revealed(board[0][1]));
}

TEST(postprocess_valid_cases)
{
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    // THEN
    fill_board_value(size, size, board, 'm');
    set_cell(&(board[0][0]), 'x');
    set_cell(&(board[0][2]), 'x');
    set_cell(&(board[2][0]), 'x');
    set_cell(&(board[2][2]), 'x');
    ASSERT(postprocess(size, size, board) == 5);
    set_cell(&(board[2][1]), 'f');
    ASSERT(postprocess(size, size, board) == 5);
    set_cell(&(board[1][1]), 'w');
    ASSERT(postprocess(size, size, board) == 4);
    fill_board_value(size, size, board, 'X');
    set_cell(&(board[0][1]), 'M');
    set_cell(&(board[1][0]), 'M');
    ASSERT(postprocess(size, size, board) == 2);
    set_cell(&(board[2][0]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
}

TEST(load_board_unrevealed)
{
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    INPUT_STRING("xxxx xxmx xxxx xmxx");
    // THEN
    ASSERT(load_board(size, size, board) == 2);
    ASSERT(!is_revealed(board[0][1]));
    ASSERT(!is_revealed(board[1][2]));
    ASSERT(!is_revealed(board[0][0]));
    set_cell(&(board[0][1]), 'F');
    ASSERT(!is_revealed(board[0][1]));
    ASSERT(!set_cell(NULL, 'F'));
    ASSERT(!set_cell(&board[0][1], NULL));
}

TEST(reveal_single_invalid)
{
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    INPUT_STRING("xmxx fxxf ww.1 ....");
    // THEN
    ASSERT(load_board(size, size, board) == 3);
    ASSERT(reveal_single(&board[1][0]) == -1);
    ASSERT(!is_revealed(board[1][0]));
    ASSERT(reveal_single(&board[3][2]) == -1);
    ASSERT(is_revealed(board[3][2]));
    ASSERT(reveal_single(&board[2][3]) == -1);
    ASSERT(is_revealed(board[2][3]));
    ASSERT(reveal_single(&board[3][3]) == -1);
    ASSERT(is_revealed(board[3][3]));
    ASSERT(reveal_single(&board[6][3]) == -1);
    ASSERT(reveal_single(&board[0][1]) == 1);
    ASSERT(is_revealed(board[0][1]));
}

TEST(show_cell_all)
{
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    INPUT_STRING(".3mx xff2 xwxx x..x");
    // THEN
    ASSERT(load_board(size, size, board) == 3);
    ASSERT(show_cell(board[0][0]) == '1');
    ASSERT(show_cell(board[0][1]) == '3');
    ASSERT(show_cell(board[0][2]) == 'X');
    ASSERT(show_cell(board[0][3]) == 'X');
    ASSERT(show_cell(board[1][2]) == 'F');
    ASSERT(show_cell(board[1][3]) == '2');
    ASSERT(show_cell(board[2][1]) == 'F');
    ASSERT(show_cell(board[3][0]) == 'X');
    ASSERT(show_cell(board[3][1]) == ' ');
    ASSERT(show_cell(board[3][2]) == ' ');
    reveal_cell(size, size, board, 0, 2);
    ASSERT(show_cell(board[0][2]) == 'M');
}

TEST(find_mines_two_neighboring) {
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxx xxxxx 1221x 1xx1x 1221x");
    ASSERT(find_mines(size, size, board) == 2);
}

TEST(find_mines_invalid_chars) {
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxx xxxdttttrastrsatsatxx 1221x 1xx1x 1221x");
    ASSERT(find_mines(size, size, board) == 2);
}

TEST(find_mines_mine_in_corner) {
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxx xxxxx xxxxx xx011 xx01x");
    ASSERT(find_mines(size, size, board) == 1);
}

TEST(find_mines_one_number) {
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("xxxxx xxxxx x8xxx xxxxx xxxxx");
    ASSERT(find_mines(size, size, board) == 8);
}

TEST(find_mines_one_number2) {
    const size_t size = 5;
    uint16_t board[5][5] = { 0 };
    INPUT_STRING("3xxxx xxxxx xxxxx xxxxx xxxxx");
    ASSERT(find_mines(size, size, board) == 3);
}

/* If you've made it down here I would also would like to tell you the meaning
 * of life is 42. */
