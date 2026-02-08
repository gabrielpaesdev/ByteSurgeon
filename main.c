/*
 * ByteSurgeon - Binary String Editor for 64-bit ELF files
 * 
 * Copyright (C) 2026 Gabriel Paes <gabrielpaesdev@proton.me>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define MIN_STR 4
#define MAX_STRINGS 1024

int is_printable(unsigned char c) {
    return (c >= 32 && c <= 126);
}

void scan_section(unsigned char *data, long offset, long size,
                  long *offsets, char **strings, int *count) {

    for (long i = 0; i < size; i++) {
        unsigned char c = data[offset + i];

        if (is_printable(c)) {
            long start = i;

            while (i < size && is_printable(data[offset + i]))
                i++;

            long len = i - start;

            if (len >= MIN_STR && i < size && data[offset + i] == 0) {
                if (*count >= MAX_STRINGS)
                    return;

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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s program\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("Error opening file");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    rewind(f);

    unsigned char *data = malloc(filesize);
    fread(data, 1, filesize, f);
    fclose(f);

    Elf64_Ehdr *eh = (Elf64_Ehdr *)data;
    Elf64_Shdr *sh = (Elf64_Shdr *)(data + eh->e_shoff);

    Elf64_Shdr *shstr = &sh[eh->e_shstrndx];
    const char *shstrtab = (const char *)(data + shstr->sh_offset);

    long offsets[MAX_STRINGS];
    char *strings[MAX_STRINGS];
    int count = 0;

    for (int i = 0; i < eh->e_shnum; i++) {
        const char *name = shstrtab + sh[i].sh_name;

        if (strcmp(name, ".rodata") == 0 || strcmp(name, ".data") == 0) {
            scan_section(data, sh[i].sh_offset, sh[i].sh_size,
                         offsets, strings, &count);
        }
    }

    if (count == 0) {
        printf("No strings found.\n");
        return 0;
    }

    int idx;
    printf("\nWhich index would you like to modify? ");
    scanf("%d", &idx);
    getchar();

    if (idx < 0 || idx >= count) {
        printf("Invalid index.\n");
        return 1;
    }

    char novo[512];
    printf("New text: ");
    fgets(novo, sizeof(novo), stdin);
    novo[strcspn(novo, "\n")] = 0;

    int old_len = strlen(strings[idx]);
    int new_len = strlen(novo);

    if (new_len > old_len) {
        printf("New text is longer than the original. Aborting.\n");
        return 1;
    }

    memcpy(&data[offsets[idx]], novo, new_len);
    memset(&data[offsets[idx] + new_len], 0, old_len - new_len);

    char outname[256];
    snprintf(outname, sizeof(outname), "%s_patched", argv[1]);

    FILE *out = fopen(outname, "wb");
    fwrite(data, 1, filesize, out);
    fclose(out);

    printf("Patch created: %s\n", outname);
    return 0;
}

