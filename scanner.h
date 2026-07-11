#ifndef SCANNER_H
#define SCANNER_H

#define MIN_STR 4
#define MAX_STRINGS 1024

int is_printable(unsigned char c);
void scan_section(unsigned char *data, long offset, long size, long *offsets, char **strings, int *count);

#endif
