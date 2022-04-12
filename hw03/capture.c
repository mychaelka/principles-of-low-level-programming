#include "capture.h"
#include <stdlib.h>
#include <string.h>


int load_capture(struct capture_t *capture, const char *filename)
{
    struct pcap_context context[1];
    if (init_context(context, filename) != PCAP_SUCCESS) {
        return -1;
    }

    capture->number_of_packets = 0;
    capture->pcap_header = malloc(sizeof(struct pcap_header_t));
    capture->packets = malloc(sizeof(struct packet_t));

    if (capture->pcap_header == NULL || capture->packets == NULL) {
        destroy_capture(capture);
        destroy_context(context);
        return -1;
    }

    memset(capture->pcap_header, 0, sizeof(struct pcap_header_t));
    memset(capture->packets, 0, sizeof(struct packet_t));

    if (load_header(context, capture->pcap_header) != PCAP_SUCCESS) {
        destroy_capture(capture);
        destroy_context(context);
        return -1;
    }

    int i = 0;
    int load_pkt_return_val;

    while (true) {
        load_pkt_return_val = load_packet(context, &capture->packets[i]);

        if (load_pkt_return_val == PCAP_FILE_END) {
            break;
        }
        if (load_pkt_return_val != 0) {
            destroy_capture(capture);
            destroy_context(context);
            return -1;
        }
        capture->number_of_packets++;
        i++;

        struct packet_t *packets_tmp = realloc(capture->packets, (i + 1) * sizeof(struct packet_t));
        if (packets_tmp == NULL) {
            destroy_capture(capture);
            destroy_context(context);
            return -1;
        }
        capture->packets = packets_tmp;

        if (capture->packets == NULL) {
            destroy_capture(capture);
            destroy_context(context);
            return -1;
        }

        memset(capture->packets + i, 0, sizeof(struct packet_t));
    }

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
    if (index > capture->number_of_packets - 1
        || capture-> number_of_packets == 0) {
        return NULL;
    }
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

// HELP FUNCTIONS
/**
 * @brief Initializes capture where filtered packets are to be stored
 *
 * @param original original capture
 * @param filtered capture to be set
 *
 * @return 0 in case of success, -1 otherwise
 */
int init_filtered_capture(
        const struct capture_t *const original,
        struct capture_t *filtered)
{
    filtered->number_of_packets = 0;
    filtered->pcap_header = malloc(sizeof(struct pcap_header_t));
    filtered->packets = malloc(sizeof(struct packet_t));

    if (filtered->pcap_header == NULL || filtered->packets == NULL) {
        free(filtered->pcap_header);
        free(filtered->packets);
        return -1;
    }

    memset(filtered->pcap_header, 0, sizeof(struct pcap_header_t));
    memcpy(filtered->pcap_header, original->pcap_header,
           sizeof(struct pcap_header_t));
    memset(filtered->packets, 0, sizeof(struct packet_t));

    return 0;
}

uint8_t right_rotate(uint8_t num, unsigned int bits)
{
    bits %= 8;
    if (bits == 0) {
        return num;
    }

    return ((num >> bits) | (num << (8 - bits)));
}

/**
 * @brief Initializes capture where filtered packets are to be stored
 *
 * @param length length of the mask in bits (max 32)
 * @param result 1D array where resulting mask will be stored,
 * divided into 4x8 bits
 *
 * @return void, modifies result param
 */
void create_mask(uint8_t length, uint8_t result[4])
{
    uint8_t mask_part = 0;
    for (int i = 0; i < 4; i++) {
        if (length == 0) {
            return;
        }
        if (length < 8) {
            mask_part = 1;
            mask_part <<= length;
            mask_part -= 1;
            mask_part = right_rotate(mask_part, length);
            result[i] = mask_part;
            return;
        }
        mask_part = UINT8_MAX;
        result[i] = mask_part;
        length -= 8;
    }
}

bool satisfies_mask(uint8_t const network_prefix[4],
                    const uint8_t mask[4],
                    const uint8_t ip[4])
{
    for (int ip_part = 0; ip_part < 4; ip_part++) {
        if ((ip[ip_part] & mask[ip_part]) != (network_prefix[ip_part] & mask[ip_part])) {
            return false;
        }
    }
    return true;
}

bool is_same_ip(const uint8_t original[4], const uint8_t copy[4])
{
    for (int ip_part = 0; ip_part < 4; ip_part++) {
        if (original[ip_part] != copy[ip_part]) {
            return false;
        }
    }
    return true;
}

void print_ip(uint8_t *ip)
{
    for (int j = 0; j < 3; j++) {
        printf("%d.", ip[j]);
    }
    printf("%d", ip[3]);
}

void print_from_to_row(uint8_t *src, uint8_t *dst)
{
    print_ip(src);
    printf(" -> ");
    print_ip(dst);
    printf(" : ");
}

bool pair_already_analysed(const struct capture_t *const capture,
                           uint8_t *src, uint8_t *dst, size_t idx)
{
    for (size_t j = 0; j < idx; j++) {
        if (is_same_ip(src, capture->packets[j].ip_header->src_addr)
            && is_same_ip(dst, capture->packets[j].ip_header->dst_addr)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Stores new timestamps from capture to two arrays
 *
 * @param start 1D array of start timestamps (sec and usec)
 * @param end 1D array of end timestamps (sec and usec)
 * @param capture capture from which the timestamps are drawn
 *
 * @return void, modifies start and end params
 */
void set_new_timestamps(uint32_t start[2], uint32_t end[2],
                        const struct capture_t *const capture)
{
    start[0] = capture->packets[0].packet_header->ts_sec;
    start[1] = capture->packets[0].packet_header->ts_usec;
    end[0] = capture->packets[capture->number_of_packets - 1].packet_header->ts_sec;
    end[1] = capture->packets[capture->number_of_packets - 1].packet_header->ts_usec;
}

int32_t get_duration_sec(const struct capture_t *const capture)
{
    return (int32_t) capture->packets[capture->number_of_packets - 1].packet_header->ts_sec \
    - (int32_t) capture->packets[0].packet_header->ts_sec;
}

int32_t get_duration_usec(const struct capture_t *const capture)
{
    return (int32_t) capture->packets[capture->number_of_packets - 1].packet_header->ts_usec \
    - (int32_t) capture->packets[0].packet_header->ts_usec;
}

// FILTERING
int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    if (init_filtered_capture(original, filtered) != 0) {
        return -1;
    }

    int copied_idx = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (original->packets[i].ip_header->protocol == protocol) {
            if (copy_packet(&original->packets[i],
                            &filtered->packets[copied_idx]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            copied_idx++;
            filtered->number_of_packets++;
            struct packet_t *packets_tmp = realloc(filtered->packets,
                                                   (copied_idx + 1) * sizeof(struct packet_t));
            if (packets_tmp == NULL) {
                destroy_capture(filtered);
                return -1;
            }
            filtered->packets = packets_tmp;
            memset(filtered->packets + copied_idx, 0, sizeof(struct packet_t));
        }
    }

    return 0;
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    if (init_filtered_capture(original, filtered) != 0) {
        return -1;
    }

    int copied_idx = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (original->packets[i].packet_header->orig_len >= size) {
            if (copy_packet(&original->packets[i],
                            &filtered->packets[copied_idx]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            copied_idx++;
            filtered->number_of_packets++;
            struct packet_t *packets_tmp = realloc(filtered->packets,
                                                   (copied_idx + 1) * sizeof(struct packet_t));
            if (packets_tmp == NULL) {
                destroy_capture(filtered);
                return -1;
            }
            filtered->packets = packets_tmp;
            memset(filtered->packets + copied_idx, 0, sizeof(struct packet_t));
        }
    }
    return 0;
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    if (init_filtered_capture(original, filtered) != 0) {
        return -1;
    }

    int copied_idx = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (is_same_ip(original->packets[i].ip_header->src_addr, source_ip)
            && is_same_ip(original->packets[i].ip_header->dst_addr, destination_ip)) {

            if (copy_packet(&original->packets[i],
                            &filtered->packets[copied_idx]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            copied_idx++;
            filtered->number_of_packets++;
            struct packet_t *packets_tmp = realloc(filtered->packets,
                                                   (copied_idx + 1) * sizeof(struct packet_t));
            if (packets_tmp == NULL) {
                destroy_capture(filtered);
                return -1;
            }
            filtered->packets = packets_tmp;
            memset(filtered->packets + copied_idx, 0, sizeof(struct packet_t));
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
    uint8_t mask[4] = { 0 };
    create_mask(mask_length, mask);

    if (init_filtered_capture(original, filtered) != 0) {
        return -1;
    }

    int copied_idx = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (satisfies_mask(network_prefix, mask,
                           original->packets[i].ip_header->src_addr)) {
            if (copy_packet(&original->packets[i],
                            &filtered->packets[copied_idx]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            copied_idx++;
            filtered->number_of_packets++;
            struct packet_t *packets_tmp = realloc(filtered->packets,
                                                   (copied_idx + 1) * sizeof(struct packet_t));
            if (packets_tmp == NULL) {
                destroy_capture(filtered);
                return -1;
            }
            filtered->packets = packets_tmp;
            memset(filtered->packets + copied_idx, 0, sizeof(struct packet_t));
        }
    }

    return 0;
}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    uint8_t mask[4] = { 0 };
    create_mask(mask_length, mask);

    if (init_filtered_capture(original, filtered) != 0) {
        return -1;
    }

    int copied_idx = 0;
    for (size_t i = 0; i < original->number_of_packets; i++) {
        if (satisfies_mask(network_prefix, mask, original->packets[i].ip_header->dst_addr)) {
            if (copy_packet(&original->packets[i],
                            &filtered->packets[copied_idx]) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            copied_idx++;
            filtered->number_of_packets++;
            struct packet_t *packets_tmp = realloc(filtered->packets,
                                                   (copied_idx + 1) * sizeof(struct packet_t));
            if (packets_tmp == NULL) {
                destroy_capture(filtered);
                return -1;
            }
            filtered->packets = packets_tmp;
            memset(filtered->packets + copied_idx, 0, sizeof(struct packet_t));
        }
    }

    return 0;
}

// STATISTICS
int print_flow_stats(const struct capture_t *const capture)
{
    struct capture_t *filtered = malloc(sizeof(struct capture_t));
    if (filtered == NULL) {
        fprintf(stderr, "Memory allocation fail\n");
        return -1;
    }

    for (size_t i = 0; i < capture->number_of_packets; i++) {
        uint8_t *src = capture->packets[i].ip_header->src_addr;
        uint8_t *dst = capture->packets[i].ip_header->dst_addr;

        if (pair_already_analysed(capture, src, dst, i)) {
            continue;
        }

        filter_from_to(capture, filtered, src, dst);
        print_from_to_row(src, dst);
        printf("%lu\n", packet_count(filtered));
        destroy_capture(filtered);
    }
    free(filtered);
    return 0;
}

int print_longest_flow(const struct capture_t *const capture)
{
    if (capture->number_of_packets == 0) {
        fprintf(stderr, "Received 0 packets.\n");
        return -1;
    }

    uint8_t flow_src_addr[4] = {0, 0, 0, 0};
    uint8_t flow_dst_addr[4] = {0, 0, 0, 0};

    // position 0. -> timestamp in sec, position 1. -> timestamp in usec
    int32_t longest_durations[2] = {0, 0};
    uint32_t start_timestamps[2] = {0, 0};
    uint32_t end_timestamps[2] = {0, 0};

    struct capture_t *filtered = malloc(sizeof(struct capture_t));
    if (filtered == NULL) {
        fprintf(stderr, "Memory allocation fail\n");
        return -1;
    }

    for (size_t i = 0; i < capture->number_of_packets; i++) {
        uint8_t *src = capture->packets[i].ip_header->src_addr;
        uint8_t *dst = capture->packets[i].ip_header->dst_addr;

        if (pair_already_analysed(capture, src, dst, i)) {
            continue;
        }

        filter_from_to(capture, filtered, src, dst);

        int32_t current_duration_sec = get_duration_sec(filtered);
        int32_t current_duration_usec = get_duration_usec(filtered);

        if (current_duration_sec == longest_durations[0]) {
            if (current_duration_usec <= longest_durations[1]
                && i > 0) { // if there is only one packet, it has the "longest duration"
                destroy_capture(filtered);
                continue;
            }
        }

        if (current_duration_sec >= longest_durations[0]) {
            for (int k = 0; k < 4; k++) {
                flow_src_addr[k] = src[k];
                flow_dst_addr[k] = dst[k];
            }
            set_new_timestamps(start_timestamps,
                               end_timestamps,
                               filtered);
            longest_durations[0] = current_duration_sec;
            longest_durations[1] = current_duration_usec;
        }
        destroy_capture(filtered);
    }

    print_from_to_row(flow_src_addr, flow_dst_addr);
    printf("%u:%u - %u:%u\n",
           start_timestamps[0],
           start_timestamps[1],
           end_timestamps[0],
           end_timestamps[1]);

    free(filtered);
    return 0;
}
