#include <stdlib.h>

#include "capture.h"

#define TEST_FILE "test.pcap"

int load_arguments(int argc, char **argv, char *filename)
{
    *filename = *argv[0];
    if (argv[2] != "flowstats" || argv[2] != "longestflow") {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *filename = NULL;

    if (argc != 4) {
        fprintf(stderr, "Invalid number of arguments!");
        return EXIT_FAILURE;
    }

    if (load_arguments(argc, argv, filename) != 0) {
        return EXIT_FAILURE;
    }

    struct pcap_context context[1];
    if (init_context(context, TEST_FILE) != PCAP_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
