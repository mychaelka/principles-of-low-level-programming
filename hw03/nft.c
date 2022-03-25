#include <stdlib.h>

#include "capture.h"

#define TEST_FILE "test.pcap"

int main()
{
    struct pcap_context context[1];
    if (init_context(context, TEST_FILE) != PCAP_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
