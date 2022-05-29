#include "cut.h"

#define main student_main
#include "../main.c"
#undef main

#include <stdlib.h>

#define ARGS(...) \
    char *argv[] = { __VA_ARGS__, NULL }; \
    int argc = (sizeof(argv) / sizeof(*argv)) - 1

TEST(sample)
{
    ARGS("checkperms", "sample.txt");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    FILE *output = fopen("sample.txt", "r");

    ASSERT_FILE(output,
        "# file: .\n"
        "# owner: xlogin\n"
        "# group: student\n"
        "# flags: ---\n"
        "user::rwxrw-r--\n"
        "group::rw-r--\n"
        "other::r--\n"
        "\n"
        "# file: test\n"
        "# owner: xlogin\n"
        "# group: student\n"
        "user::rwxrwx---\n"
        "group::rwx---\n"
        "other::---\n"
        "\n"
        "# file: test/run\n"
        "# owner: root\n"
        "# group: root\n"
        "user::rwxrwxrwx\n"
        "group::rwxrwx\n"
        "other::rwx\n"
    );

    fclose(output);
}
