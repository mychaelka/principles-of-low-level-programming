#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// Do *NOT* use ‹readline()› in this exercise! It will not work as you expect.
// #include "readline.h"

#define UNUSED(VAR) \
    ((void) (VAR))

/**
 * Reads 512 binary bytes from \p key and uses it as key to "encrypt"
 * each block of 512 from \p in. Resulting blocks are written to \p out.
 *
 * @param   key         the file to read key from
 * @param   in          input file
 * @param   out         output file
 * @return  0 on success, an arbitrary error code on failure
 */
int xorcrypt(FILE *key, FILE *in, FILE *out)
{
    char key_[512];
    char buffer[512];

    if (fread(key_, 512, 1, key) != 1 || ferror(key)) {
        fprintf(stderr, "Error occured");
        return -1;
    }

    while (fread(buffer, 512, 1, in) != 0) {
        char xor[512];
        for (int i = 0; i < 512; i++) {
            xor[i] = buffer[i] ^ key_[i];
        }
        fwrite(xor, 512, 1, out);
        fseek(in, 512, SEEK_CUR);
    }
    // 1) read the key to a 512 byte long buffer using ‹fread()›
    // 2) read 512 byte blocks from ‹in› file
    // 3) for each such block use the xor operator ‹^› (caret) to encrypt
    //    each byte of the block with the respective byte in the ‹key› block
    // 4) write the modified block to the ‹out› file
    //
    // Note that the last block may be smaller than 512 bytes.
    // You sould take this into account when implementing this function.
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s KEY INPUT OUTPUT\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[2], "rb");
    FILE *key = fopen(argv[1], "rb");

    if (in == NULL) {
        fputs("Cannot open file", stderr);
        return EXIT_FAILURE;
    }

    FILE *out = fopen(argv[3], "w");
    if (out == NULL) {
        fclose(in);
        fputs("Cannot open file", stderr);
        return EXIT_FAILURE;
    }

    if (xorcrypt(key, in, out) != 0) {
        fputs("Error occured", stderr);
        return EXIT_FAILURE;
    }

    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
