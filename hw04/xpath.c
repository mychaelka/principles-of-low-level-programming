#include "xpath.h"
#include "xparser.h"

struct result_text {
    char *text;
};

struct result_xml  {
    char *elements;
};

bool is_valid_xpath(char *xpath)
{
    if (xpath[0] != '/') {
        fprintf(stderr, "Query: expected '/' but got '%c' instead\n", xpath[0]);
        return false;
    }

    return true;
}

void parse_xpath(char *xpath);