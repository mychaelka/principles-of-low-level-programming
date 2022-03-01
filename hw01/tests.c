
#define CUT_MAIN
#include "cut.h"

#define main student_main
#include "main.c"
#undef main

TEST(nanecisto_1)
{
    INPUT_STRING(
            "; na vstupu se nenachazi zadny prikaz, komentar tedy nevyvola zadny vypis\n"
            "=\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 0\n"
            "");
}

TEST(nanecisto_2)
{
    INPUT_STRING(
            "P\n"
            "10\n"
            "\n"
            "43\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 1043\n"
            "");
}

TEST(nanecisto_binary)
{
    INPUT_STRING(
            "P T 110\n"
            "=\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 0\n"
            "# 6\n"
            "# 6\n"
            "");
}

TEST(nanecisto_minus)
{
    INPUT_STRING(
            "P 100\n"
            "- 50\n"
            "=");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 100\n"
            "# 50\n"
            "# 50\n"
            "");
}

TEST(nanecisto_octal)
{
    INPUT_STRING(
            "P O 72\n"
            "=\n"
            "O\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 0\n"
            "# 58\n"
            "# 58\n"
            "# 72\n"
            "");
}

TEST(nanecisto_zadani_3)
{
    INPUT_STRING(
            "P 21\n"
            "; okamzite provedeni prikazu vyse - ukonceni predchoziho vstupu\n"
            "\n"
            "P 22\n"
            "=\n"
            "\n"
            "; jednotlive prikazy nemusi byt oddeleny novym radkem\n"
            "P 23 =\n"
            "\n"
            "N\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 21\n"
            "# 22\n"
            "# 22\n"
            "# 23\n"
            "# 23\n"
            "# 0\n"
            "");
}

TEST(nanecisto_zadani_4)
{
    INPUT_STRING(
            "P 200 ;\n"
            "/ 5 ;\n"
            "% 6 ;\n"
            "=\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 200\n"
            "# 40\n"
            "# 4\n"
            "# 4\n"
            "");
}

TEST(nanecisto_zadani_5)
{
    INPUT_STRING(
            "P 10 ;\n"
            "< 3 ;\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 10\n"
            "# 80\n"
            "");
}

TEST(nanecisto_zadani_6)
{
    INPUT_STRING(
            "P 21 + 43 * 5 =\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 21\n"
            "# 64\n"
            "# 320\n"
            "# 320\n"
            "");
}

TEST(nanecisto_zadani_7)
{
    INPUT_STRING(
            "P 15 ;\n"
            "* X 1a ;\n"
            "O ;\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 15\n"
            "# F\n"
            "# 390\n"
            "# 606\n"
            "");
}

TEST(nanecisto_zadani_8)
{
    INPUT_STRING(
            "P 3 ;    akumulator = 3, pamet = 0\n"
            "M   ;    akumulator = 3, pamet = 3\n"
            "R   ;    akumulator = 3, pamet = 0\n"
            "M   ;    akumulator = 3, pamet = 3\n"
            "+ m ;    akumulator = 3 + 3 = 6, pamet = 3\n"
            "=\n"
            "");

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
            "# 3\n"
            "# 6\n"
            "# 6\n"
            "");
}

TEST(my_test_1)
{
    INPUT_STRING(
            "P 10 + 7 ;\n"
            );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 10\n"
                "# 17\n"
                );
}

TEST(my_test_2)
{
    INPUT_STRING(
            "a ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_3)
{
    INPUT_STRING(
            "P 50 + 7 a ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 50\n"
                "# 57\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}
TEST(my_test_4)
{
    INPUT_STRING(
            "20 + 3 ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}
TEST(my_test_5)
{
    INPUT_STRING(
            "P 4 - X 8 ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 4\n"
                "# 4\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Out of range\n"
                ""
    );
}

TEST(my_test_6)
{
    INPUT_STRING(
            "X 20\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_7)
{
    INPUT_STRING(
            "P T 80 ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_8)
{
    INPUT_STRING(
            "P X X ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}
TEST(my_test_9)
{
    INPUT_STRING(
            "PXO;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_10)
{
    INPUT_STRING(
            "X =\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 0\n"
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_11)
{
    INPUT_STRING(
            "X 20\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_12)
{
    INPUT_STRING(
            "X 1 O;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_13)
{
    INPUT_STRING(
            "Pm;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_14)
{
    INPUT_STRING(
            "POm;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_15)
{
    INPUT_STRING(
            "Pmm;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_16)
{
    INPUT_STRING(
            "+ T 110;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 0\n"
                "# 6\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_17)
{
    INPUT_STRING(
            "P 50 / X 1af ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 50\n"
                "# 32\n"
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_18)
{
    INPUT_STRING(
            "P O 10m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                "# 8\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_19)
{
    INPUT_STRING(
            "P10m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 10\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_20)
{
    INPUT_STRING(
            "P m 30 ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_21)
{
    INPUT_STRING(
            "P 15; MRRM; +m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 15\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_22)
{
    INPUT_STRING(
            "P 2 M ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 2\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_23)
{
    INPUT_STRING(
            "M;P10;/m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_24)
{
    INPUT_STRING(
            "M;\nP 10;\n/m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 10\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Division by zero\n"
                ""
    );
}

TEST(my_test_25)
{
    INPUT_STRING(
            "P3;\n M;\n P10;\n <m;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 3\n"
                "# 10\n"
                "# 80\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_26)
{
    INPUT_STRING(
            "P 51 + 2 R =\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 51\n"
                "# 53\n"
                "# 53\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_27)
{
    INPUT_STRING(
            "Pm M R ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_28)
{
    INPUT_STRING(
            "P 10 + m m  ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 10\n"
                "# 10\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_29)
{
    INPUT_STRING(
            "P 3 X 11 ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 3\n"
                "# 3\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_30)
{
    INPUT_STRING(
            "P 15 X ;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 15\n"
                "# F\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_31)
{
    INPUT_STRING(
            "P63223\n;R\nXRNRNMNONXX;NMPMRX\n1010M T"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 63223\n"
                "# F6F7\n"
                "# 0\n"
                "# 0\n"
                "# 0\n"
                "# 0\n"
                "# 0\n"
                "# 0\n"
                "# 0\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_32)
{
    INPUT_STRING(
            "P16430=XOM=101/N+N=R\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 16430\n"
                "# 16430\n"
                "# 402E\n"
                "# 40056\n"
                "# 16430\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Syntax error\n"
                ""
    );
}

TEST(my_test_33)
{
    INPUT_STRING(
            "P1;<63=\n"
            ""
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 1\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_34)
{
    INPUT_STRING(
            "P1<63="
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 1\n"
                "# 9223372036854775808\n"
                "# 9223372036854775808\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_35)
{
    INPUT_STRING(
            "PO57PO11;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 0);
    ASSERT_FILE(stdout,
                "# 0\n"
                "# 47\n"
                "# 57\n"
                "# 9\n"
                ""
    );
    ASSERT_FILE(stderr,
                ""
    );
}

TEST(my_test_36)
{
    INPUT_STRING(
            "P1;\n"
            "<64;\n"
    );

    int main_rv = student_main();

    ASSERT(main_rv == 1);
    ASSERT_FILE(stdout,
                "# 1\n"
                ""
    );
    ASSERT_FILE(stderr,
                "Out of range"
                ""
    );
}
