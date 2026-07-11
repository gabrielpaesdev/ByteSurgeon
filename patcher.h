#ifndef PATCHER_H
#define PATCHER_H

int apply_patch(unsigned char *data, long offset, const char *old_str, const char *new_str);
void save_patched_file(const char *original_name, unsigned char *data, long filesize);

#endif
