#include <stdio.h>
#include <stdlib.h>
#include "capture.h"

#define TEST_FILE "test.pcap"
//#define TEST_FILE "mytest.pcap"

void demo1()
{
    struct pcap_context context[1];
    if (init_context(context, TEST_FILE) != PCAP_SUCCESS) {
        return;
    }

    struct pcap_header_t *pcap_header = malloc(sizeof(struct pcap_header_t));
    if (load_header(context, pcap_header) != PCAP_SUCCESS) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    struct packet_t *packet1 = malloc(sizeof(struct packet_t));

    if (packet1 == NULL) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    if (load_packet(context, packet1) != PCAP_SUCCESS) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    struct packet_t *packet2 = malloc(sizeof(struct packet_t));

    if (packet2 == NULL) {
        free(packet1);
        free(pcap_header);
        destroy_context(context);
        return;
    }

    if (load_packet(context, packet2) != PCAP_SUCCESS) {
        free(packet1);
        free(pcap_header);
        destroy_context(context);
        return;
    }

    destroy_context(context);

    printf("packet 1:\n");
    print_packet_info(packet1);

    printf("\npacket 2:\n");
    print_packet_info(packet2);

    destroy_packet(packet1);
    destroy_packet(packet2);

    free(packet1);
    free(packet2);
    free(pcap_header);
}

void demo2()
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    for (size_t current_packet = 0; current_packet < packet_count(capture); current_packet++) {
        struct packet_t *packet = get_packet(capture, current_packet);
        print_packet_info(packet);
    }

    printf("Magic number: 0x%x\n", get_header(capture)->magic_number);
    printf("Packet count: %zu\n", packet_count(capture));
    printf("Total number of bytes transferred in this capture: %zu.\n", data_transfered(capture));


    struct capture_t *filtered = malloc(sizeof(struct capture_t));
    filter_protocol(capture, filtered, 6);

    printf("Magic number: 0x%x\n", get_header(filtered)->magic_number);
    printf("Packet count: %zu\n", packet_count(filtered));
    printf("Total number of bytes transferred in this capture: %zu.\n", data_transfered(filtered));


    struct capture_t *bigger_than = malloc(sizeof(struct capture_t));
    filter_larger_than(filtered, bigger_than, 90);

    printf("Magic number: 0x%x\n", get_header(bigger_than)->magic_number);
    printf("Packet count: %zu\n", packet_count(bigger_than));
    printf("Total number of bytes transferred in this capture: %zu.\n", data_transfered(bigger_than));

    destroy_capture(bigger_than);
    free(bigger_than);


    struct capture_t *filtered_from_to = malloc(sizeof(struct capture_t));
    filter_from_to(
            capture,
            filtered_from_to,
            (uint8_t[4]){ 74U, 125U, 19U, 17U },
            (uint8_t[4]){ 172U, 16U, 11U, 12U });

    printf("Packet count filtered from to: %zu\n", packet_count(filtered_from_to));
    destroy_capture(filtered);
    free(filtered);
    destroy_capture(filtered_from_to);
    free(filtered_from_to);
    destroy_capture(capture);
    free(capture);
}


void demo3()
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    uint8_t network_prefix[4] = {216, 34, 181, 45};
    struct capture_t *filtered_from_mask = malloc(sizeof(struct capture_t));
    filter_from_mask(capture, filtered_from_mask, network_prefix, 32);

    for (size_t current_packet = 0; current_packet < packet_count(filtered_from_mask); current_packet++) {
        struct packet_t *packet = get_packet(filtered_from_mask, current_packet);
        print_packet_info(packet);
    }

    printf("Magic number: 0x%x\n", get_header(filtered_from_mask)->magic_number);
    printf("Packet count: %zu\n", packet_count(filtered_from_mask));
    printf("Total number of bytes transferred in this capture: %zu.\n", data_transfered(filtered_from_mask));

    print_flow_stats(capture);


    destroy_capture(capture);
    free(capture);
    destroy_capture(filtered_from_mask);
    free(filtered_from_mask);
}

void demo4()
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    print_longest_flow(capture);

    for (size_t current_packet = 0; current_packet < packet_count(capture); current_packet++) {
        struct packet_t *packet = get_packet(capture, current_packet);
        print_packet_info(packet);
    }

    destroy_capture(capture);
    free(capture);
}

int main()
{
    demo1();
    demo2();
    demo3();
    demo4();
}
