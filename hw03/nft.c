#include <stdlib.h>
#include <string.h>

#include "capture.h"

#define TEST_FILE "test.pcap"

struct arguments
{
    char *filename;
    uint8_t src[4];
    uint8_t dst[4];
    uint8_t from_bits;
    uint8_t to_bits;
    bool flowstats;
};

int load_arguments(char **argv, struct arguments *args)
{
    args->filename = argv[1];

    if (strcmp(argv[4], "flowstats") == 0) {
        args->flowstats = true;
    } else if (strcmp(argv[4], "longestflow") == 0) {
        args->flowstats = false;
    } else {
        fprintf(stderr, "Third argument must be one of 'flowstats/longestflow'\n");
        return -1;
    }

    for (int ip_part = 0; ip_part < 4; ip_part++) {
        args->src[ip_part] = 0;
        args->dst[ip_part] = 0;
    }

    int ret_val_from = sscanf(argv[2], "%hhu.%hhu.%hhu.%hhu/%hhu",
                              &args->src[0], &args->src[1], &args->src[2], &args->src[3], &args->from_bits);
    int ret_val_to = sscanf(argv[3], "%hhu.%hhu.%hhu.%hhu/%hhu",
                            &args->dst[0], &args->dst[1], &args->dst[2], &args->dst[3], &args->to_bits);

    if (ret_val_from != 5 || ret_val_to != 5) {
        fprintf(stderr, "Wrong from/to address!\n");
        return -1;
    }

    if (args->from_bits > 32 || args->to_bits > 32) {
        fprintf(stderr, "Couldn't parse mask\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Invalid number of arguments!\n");
        return EXIT_FAILURE;
    }

    struct arguments args = {" ", {0U, 0U, 0U, 0U},
            {0U, 0U, 0U, 0U}, 0, 0, false};

    if (load_arguments(argv, &args) != 0) {
        return EXIT_FAILURE;
    }

    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, args.filename);

    struct capture_t *filtered_from = malloc(sizeof(struct capture_t));
    filter_from_mask(capture, filtered_from, args.src, args.from_bits);

    struct capture_t *filtered_to = malloc(sizeof(struct capture_t));
    filter_to_mask(filtered_from, filtered_to, args.dst, args.to_bits);

    if (args.flowstats) {
        print_flow_stats(filtered_to);
    }
    else {
        print_longest_flow(filtered_to);
    }

    destroy_capture(capture);
    free(capture);
    destroy_capture(filtered_from);
    free(filtered_from);
    destroy_capture(filtered_to);
    free(filtered_to);
    return EXIT_SUCCESS;
}
