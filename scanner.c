#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

int is_printable(unsigned char c) {
    return (c >= 32 && c <= 126);
}

void scan_section(unsigned char *data, long offset, long size, long *offsets, char **strings, int *count) {
    for (long i = 0; i < size; i++) {
        unsigned char c = data[offset + i];

        if (is_printable(c)) {
            long start = i;

            while (i < size && is_printable(data[offset + i])) i++;

            long len = i - start;

            if (len >= MIN_STR && i < size && data[offset + i] == 0) {
                if (*count >= MAX_STRINGS) return;

                strings[*count] = malloc(len + 1);
                memcpy(strings[*count], &data[offset + start], len);
                strings[*count][len] = 0;

                offsets[*count] = offset + start;
                printf("[%d] %s\n", *count, strings[*count]);
                (*count)++;
            }
        }
    }
}
