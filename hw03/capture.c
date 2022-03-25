#include "capture.h"
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) ((void) x)

int load_capture(struct capture_t *capture, const char *filename)
{
    // initialize the pcap context - open the file
    struct pcap_context context[1];
    if (init_context(context, filename) != PCAP_SUCCESS) {
        destroy_context(context);
        return -1;
    }

    // initialize pcap_header part of main struct
    capture->number_of_packets = 0;
    capture->pcap_header = malloc(sizeof(struct pcap_header_t));
    memset(capture->pcap_header, 0, sizeof(struct pcap_header_t));

    if (load_header(context, capture->pcap_header) != PCAP_SUCCESS) {
        destroy_capture(capture);
        destroy_context(context);
        return -1;
    }

    // start loading packets into struct
    int i = 0;
    int load_pkt_return_val = 0;
    capture->packets = malloc(sizeof(struct packet_t));
    memset(capture->packets, 0, sizeof(struct packet_t));

    // find out a way to load all packets (when does loading stop?)
    while (true) {
        load_pkt_return_val = load_packet(context, &capture->packets[i]);

        if (load_pkt_return_val == PCAP_FILE_END) {
            break;
        }
        // if loading fails before file end, free memory
        if (load_pkt_return_val != 0) {
            destroy_capture(capture);
            destroy_context(context);
            return -1;
        }
        capture->number_of_packets++; // increase number of loaded packets
        i++;
        capture->packets = realloc(capture->packets, (i + 1) * sizeof(struct packet_t)); // space for new pkt
        memset(capture->packets + i, 0, sizeof(struct packet_t));
    }

    // destroy loaded pcap context at the end
    destroy_context(context);
    return 0;
}

void destroy_capture(struct capture_t *capture)
{
    free(capture->pcap_header);
    for (size_t i = 0; i < capture->number_of_packets; i++) {
        destroy_packet(&capture->packets[i]);
    }
    free(capture->packets);
}

const struct pcap_header_t *get_header(const struct capture_t *const capture)
{
    return capture->pcap_header;
}
struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index)
{
    return &capture->packets[index];
}

size_t packet_count(const struct capture_t *const capture)
{
    return capture->number_of_packets;
}

size_t data_transfered(const struct capture_t *const capture)
{
    size_t transfer = 0;
    for (size_t i = 0; i < capture->number_of_packets; i++) {
        transfer += capture->packets[i].packet_header->orig_len;
    }

    return transfer;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    filtered->pcap_header = malloc(sizeof(struct pcap_header_t));
    memset(filtered->pcap_header, 0, sizeof(struct pcap_header_t));

    // copy pcap header (is there a better way?)
    memcpy(filtered->pcap_header, original->pcap_header, sizeof(struct pcap_header_t));

    filtered->packets = malloc(sizeof(struct packet_t));
    memset(filtered->packets, 0, sizeof(struct packet_t));

    int j = 0;
    filtered->number_of_packets = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (original->packets[i].ip_header->protocol == protocol) {
            if (copy_packet(&original->packets[i], &filtered->packets[j]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            j++;
            filtered->number_of_packets++;
            filtered->packets = realloc(filtered->packets, (j + 1) * sizeof(struct packet_t));
            memset(filtered->packets + j, 0, sizeof(struct packet_t));
        }
    }
    return 0;
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    filtered->pcap_header = malloc(sizeof(struct pcap_header_t));
    memset(filtered->pcap_header, 0, sizeof(struct pcap_header_t));

    // copy pcap header (is there a better way?)
    memcpy(filtered->pcap_header, original->pcap_header, sizeof(struct pcap_header_t));

    filtered->packets = malloc(sizeof(struct packet_t));
    memset(filtered->packets, 0, sizeof(struct packet_t));

    int j = 0;
    filtered->number_of_packets = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (original->packets[i].packet_header->orig_len >= size) {
            if (copy_packet(&original->packets[i], &filtered->packets[j]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            j++;
            filtered->number_of_packets++;
            filtered->packets = realloc(filtered->packets, (j + 1) * sizeof(struct packet_t));
            memset(filtered->packets + j, 0, sizeof(struct packet_t));
        }
    }
    return 0;
}

// can I use a helper function?
bool is_same_ip(uint8_t original[4], uint8_t copy[4])
{
    for (int ip_part = 0; ip_part < 4; ip_part++) {
        if (original[ip_part] != copy[ip_part]) {
            return false;
        }
    }
    return true;
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    filtered->pcap_header = malloc(sizeof(struct pcap_header_t));
    memset(filtered->pcap_header, 0, sizeof(struct pcap_header_t));

    // copy pcap header (is there a better way?)
    memcpy(filtered->pcap_header, original->pcap_header, sizeof(struct pcap_header_t));

    filtered->packets = malloc(sizeof(struct packet_t));
    memset(filtered->packets, 0, sizeof(struct packet_t));

    int j = 0;
    filtered->number_of_packets = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (is_same_ip(original->packets[i].ip_header->src_addr, source_ip)
            && is_same_ip(original->packets[i].ip_header->dst_addr, destination_ip)) {

            if (copy_packet(&original->packets[i], &filtered->packets[j]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            j++;
            filtered->number_of_packets++;
            filtered->packets = realloc(filtered->packets, (j + 1) * sizeof(struct packet_t));
            memset(filtered->packets + j, 0, sizeof(struct packet_t));
        }
    }
    return 0;
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
