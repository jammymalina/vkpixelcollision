#ifndef PATH_TOOLS_H
#define PATH_TOOLS_H

#include <stdbool.h>
#include <stddef.h>

#define PATH_MAX_SIZE 4096
#define PATH_MAX_EXTENSION_SIZE 32

bool path_retrieve_basepath(char basepath[PATH_MAX_SIZE]);
bool path_extract_extension(const char path[PATH_MAX_SIZE],
    char extension[PATH_MAX_EXTENSION_SIZE]);

#endif
