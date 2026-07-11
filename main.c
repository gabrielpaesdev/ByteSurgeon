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
#include "scanner.h"
#include "patcher.h"

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
    if (!data) {
        perror("Memory allocation failed");
        fclose(f);
        return 1;
    }
    
    fread(data, 1, filesize, f);
    fclose(f);

    Elf64_Ehdr *eh = (Elf64_Ehdr *)data;

    if (filesize < sizeof(Elf64_Ehdr) || memcmp(eh->e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Error: File is not a valid ELF binary.\n");
        free(data);
        return 1;
    }
    
    if (eh->e_ident[EI_CLASS] != ELFCLASS64) {
        printf("Error: ByteSurgeon only supports 64-bit ELF binaries.\n");
        free(data);
        return 1;
    }

    Elf64_Shdr *sh = (Elf64_Shdr *)(data + eh->e_shoff);
    Elf64_Shdr *shstr = &sh[eh->e_shstrndx];
    const char *shstrtab = (const char *)(data + shstr->sh_offset);

    long offsets[MAX_STRINGS];
    char *strings[MAX_STRINGS];
    int count = 0;

    for (int i = 0; i < eh->e_shnum; i++) {
        const char *name = shstrtab + sh[i].sh_name;

        if (strcmp(name, ".rodata") == 0 || strcmp(name, ".data") == 0) {
            scan_section(data, sh[i].sh_offset, sh[i].sh_size, offsets, strings, &count);
        }
    }

    if (count == 0) {
        printf("No strings found.\n");
        free(data);
        return 0;
    }

    int idx;
    printf("\nWhich index would you like to modify? ");
    if (scanf("%d", &idx) != 1) {
        printf("Invalid input.\n");
        for (int i = 0; i < count; i++) free(strings[i]);
        free(data);
        return 1;
    }
    getchar();

    if (idx < 0 || idx >= count) {
        printf("Invalid index.\n");
        for (int i = 0; i < count; i++) free(strings[i]);
        free(data);
        return 1;
    }

    char novo[512];
    printf("New text: ");
    fgets(novo, sizeof(novo), stdin);
    novo[strcspn(novo, "\n")] = 0;

    if (!apply_patch(data, offsets[idx], strings[idx], novo)) {
        printf("New text is longer than the original. Aborting.\n");
        for (int i = 0; i < count; i++) free(strings[i]);
        free(data);
        return 1;
    }

    save_patched_file(argv[1], data, filesize);

    for (int i = 0; i < count; i++) {
        free(strings[i]);
    }
    free(data);

    return 0;
}
