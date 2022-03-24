#include "capture.h"
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) ((void) x)

int load_capture(struct capture_t *capture, const char *filename)
{
    // initialize the pcap context - open the file
    struct pcap_context context[1];
    if (init_context(context, filename) != PCAP_SUCCESS) {
        return -1;
    }

    // initialize pcap_header part of main struct
    capture->pcap_header = malloc(sizeof(struct pcap_header_t));
    memset(capture->pcap_header, 0, sizeof(struct pcap_header_t));

    if (load_header(context, capture->pcap_header) != PCAP_SUCCESS) {
        free(capture->pcap_header);
        destroy_context(context);
        return -1;
    }

    // start loading packets into struct
    load_packet(context, &capture->packets[0]);
    int i = 1;
    int load_pkt_return_val = 0;

    // find out a way to load all packets (when does loading stop?)
    while (load_pkt_return_val != PCAP_FILE_END) {
        // load packets
        load_pkt_return_val = load_packet(context, &capture->packets[i]);
        // if return value is failure, free memory
        i++;
    }

    // destroy loaded pcap context at the end
    destroy_context(context);

    return 0;
}

void destroy_capture(struct capture_t *capture)
{
    UNUSED(capture);
}

const struct pcap_header_t *get_header(const struct capture_t *const capture)
{
    UNUSED(capture);
    return NULL;
}
struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index)
{
    UNUSED(capture);
    UNUSED(index);
    return NULL;
}

size_t packet_count(const struct capture_t *const capture)
{
    return capture->number_of_packets;
}

size_t data_transfered(const struct capture_t *const capture)
{
    UNUSED(capture);
    return 0;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(protocol);
    return -1;
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(size);
    return -1;
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(source_ip);
    UNUSED(destination_ip);
    return -1;
}

int filter_from_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(network_prefix);
    UNUSED(mask_length);
    return -1;
}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    UNUSED(original);
    UNUSED(filtered);
    UNUSED(network_prefix);
    UNUSED(mask_length);
    return -1;
}

int print_flow_stats(const struct capture_t *const capture)
{
    UNUSED(capture);
    return -1;
}

int print_longest_flow(const struct capture_t *const capture)
{
    UNUSED(capture);
    return -1;
}
