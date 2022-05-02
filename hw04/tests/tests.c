#include "cut.h"

#define main student_main
#include "../main.c"
#undef main

#include <stdlib.h>
#include "utils_for_tests.h"

#define ARGS(...) \
    char *argv[] = { __VA_ARGS__, NULL }; \
    int argc = (sizeof(argv) / sizeof(*argv)) - 1

/* Provided tests are testing your outputs in two ways:
 *
 * 1) Text format is tested on exact match on stdout via macro ASSERT_FILE().
 *
 * 2) XML format is stored in file 'out.xml' (which is later removed)
 *    and tested with special function match().
 *    As there is no strict formating rule on your XML in assignment,
 *    mentioned function compares your xml with special pattern.
 *
 *    Pattern:
 *    - where can be 0..N whitespaces, whitespaces in patern ends with '*'
 *    - where can be 1..N whitespaces, whitespaces in patern ends with '+'
 *    - where can be 0..1 whitespaces, whitespaces in patern ends with '?' (not used)
 *
 * You can write your own tests in similar manner.
 **/

TEST(nanecisto_linear)
// basic xml, text output to stdout on /bookstore
{
    INPUT_STRING(
        "<bookstore>\n"
        "  <book category=\"cooking\">\n"
        "    <title lang=\"en\">Everyday Italian</title>\n"
        "  </book>\n"
        "</bookstore>"
    );

    ARGS( "xpath", "-t", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    ASSERT_FILE(stdout,
        "Everyday Italian\n"
    );
}

TEST(nanecisto_zadani)
// from assignment, xml output to file on /bookstore/book
{
    INPUT_STRING(
        "<bookstore>\n"
        "  <book category=\"cooking\">\n"
        "    <title lang=\"en\">Everyday Italian</title>\n"
        "    <author>Giada De Laurentiis</author>\n"
        "    <year>2005</year>\n"
        "    <price>30.00</price>\n"
        "  </book>\n"
        "</bookstore>\n"
    );

    const char *test_model =
        "  *< *book +category *= *\"cooking\" *>"
        "    *< *title +lang *= *\"en\" *> *Everyday +Italian *</ *title *>"
        "    *< *author *> *Giada +De +Laurentiis *</ *author *>"
        "    *< *year *> *2005 *</ *year *>"
        "    *< *price *> *30.00 *</ *price *>"
        "  *</ *book *>"
    ;

    char *student_file = "out.xml";
    ARGS( "xpath", "-x", "-o", student_file, "/bookstore/book" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    char *student_str = NULL;
    ASSERT(!file_to_string(student_file, &student_str));
    int test_retval = match(student_str, test_model);
    free(student_str);
    remove(student_file);
    ASSERT(test_retval == 1);
}

TEST(nanecisto_zadani_vic_attrs)
// xml with more attributes in node, xml output to file on /bookstore
{
    INPUT_STRING(
        "  <bookstore >\n"
        "  <book category = \"cooking\" colored = \"true\" >\n"
        "    <title lang = \"en\" > Everyday Italian </title >\n"
        "    <author > Giada De Laurentiis </author >\n"
        "    <year > 2005 </year >\n"
        "    <price > 30.00 </price >\n"
        "  </book >\n"
        "  </bookstore          >\n"
    );

    const char *test_model =
        "  *< *bookstore *>"
        "  *< *book +category *= *\"cooking\" +colored *= *\"true\" *>"
        "    *< *title +lang *= *\"en\" *> *Everyday +Italian *</ *title *>"
        "    *< *author *> *Giada +De +Laurentiis *</ *author *>"
        "    *< *year *> *2005 *</ *year *>"
        "    *< *price *> *30.00 *</ *price *>"
        "  *</ *book *>"
        "  *</ *bookstore *>"
    ;

    char *student_file = "out.xml";
    ARGS( "xpath", "-x", "-o", student_file, "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    char *student_str = NULL;
    ASSERT(!file_to_string(student_file, &student_str));
    int test_retval = match(student_str, test_model);
    free(student_str);
    remove(student_file);
    ASSERT(test_retval == 1);
}

TEST(multiple_input)
{
    ARGS( "xpath", "-x", "-i", "bookstore.xml", "-i", "bookstore.xml","/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(same_input_output)
{
    INPUT_STRING(
            "  <bookstore >\n"
            "  <book category = \"cooking\" colored = \"true\" >\n"
            "    <title lang = \"en\" > Everyday Italian </title >\n"
            "    <author > Giada De Laurentiis </author >\n"
            "    <year > 2005 </year >\n"
            "    <price > 30.00 </price >\n"
            "  </book >\n"
            "  </bookstore          >\n"
    );

    ARGS( "xpath", "-x", "-o", "out.xml", "-i", "out.xml","/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(multiple_output)
{
    ARGS( "xpath", "-x", "-o", "out.xml", "-o", "out.xml","/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(multiple_output2)
{
    ARGS( "xpath", "-o", "out.xml", "-o", "out.xml", "-x", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(input_long)
{
    ARGS( "xpath", "--input", "bookstore.xml", "-x", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(x_and_t)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "-x", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(xml_weird_names)
{
    ARGS( "xpath", "-x", "--input", "no_letters.xml", "/_45:../_-45:../_.--" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(wrong_args)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "-y", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(multiple_roots)
{
    ARGS( "xpath", "-t", "--input", "multiple_roots.xml", "/bookstore" );
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(empty_xpath)
{
    ARGS( "xpath", "-t", "--input", "multiple_roots.xml");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
    ASSERT_FILE(stderr, "Positional argument XPATH required\n");
}

TEST(missing_end_tag)
{
    ARGS( "xpath", "-t", "--input", "no_end_tag.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(different_end_tag)
{
    ARGS( "xpath", "-t", "--input", "different_end_tag.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(whitespaces_good)
{
    ARGS( "xpath", "-t", "--input", "whitespaces.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(whitespaces_good_bad)
{
    ARGS( "xpath", "-t", "--input", "wrong_whitespaces.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(empty_name)
{
    ARGS( "xpath", "-t", "--input", "empty_name.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(empty_text)
{
    ARGS( "xpath", "-t", "--input", "empty_text.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(special_character_in_text)
{
    ARGS( "xpath", "-t", "--input", "bad_text.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(not_unique_attributes)
{
    ARGS( "xpath", "-t", "--input", "no_unique_attributes.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(not_spaces_between_attributes)
{
    ARGS( "xpath", "-t", "--input", "no_spaces_between_attributes.xml", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(xpath_zero_index)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "/bookstore/book[0]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(xpath_big_number_success)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "/bookstore/book[45]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(xpath_big_number_overflow)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "/bookstore/book[65446568345546835463534658]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(xpath_negative_index)
{
    ARGS( "xpath", "-t", "--input", "bookstore.xml", "/bookstore/book[-4]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(xpath_correct_attribute)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@category=\"web\"]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(xpath_nonexistent_attribute)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@category=\"hehe\"]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(attribute_without_value)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@category=]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(attribute_without_key)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@=\"category\"]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(attribute_without_quotation_marks)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@category=web]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(attribute_whitespace_around_equal)
{
    ARGS( "xpath", "-x", "--input", "bookstore.xml", "/bookstore/book[@category=   \"web]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(index_middle_of_xpath)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[1]/title");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(attribute_middle_of_xpath)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@category=\"web\"]/title");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(multiple_attributes)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@category=\"web\"]/title[@lang=\"en\"]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
    ASSERT_FILE(stdout, "XQuery Kick Start\n"
                        "Learning XML\n");
}

TEST(multiple_indices)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[1]/title[2]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(multiple_indices2)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[1]/title[1]");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
    ASSERT_FILE(stdout, "Everyday Italian\n");
}

TEST(slash_at_the_end)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book/");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(slash_at_the_end_attr)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@category]/");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(slash_at_the_end_index)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@category=\"web\"]/");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(slash_at_the_end_attr_with_val)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[1]/");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 1);
}

TEST(wildcard1)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/*/title");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(wildcard2)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@*]/title");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}

TEST(wildcard3)
{
    ARGS( "xpath",  "--input", "bookstore.xml", "/bookstore/book[@*=\"children\"]/title");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);
}