#include <stdio.h>
#include <string.h>
#include "patcher.h"

int apply_patch(unsigned char *data, long offset, const char *old_str, const char *new_str) {
    int old_len = strlen(old_str);
    int new_len = strlen(new_str);

    if (new_len > old_len) {
        return 0;
    }

    memcpy(&data[offset], new_str, new_len);
    memset(&data[offset + new_len], 0, old_len - new_len);
    
    return 1;
}

void save_patched_file(const char *original_name, unsigned char *data, long filesize) {
    char outname[256];
    snprintf(outname, sizeof(outname), "%s_patched", original_name);

    FILE *out = fopen(outname, "wb");
    if (out) {
        fwrite(data, 1, filesize, out);
        fclose(out);
        printf("Patch created: %s\n", outname);
    } else {
        perror("Error saving patched file");
    }
}
