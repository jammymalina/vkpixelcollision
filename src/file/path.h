#ifndef PATH_TOOLS_H
#define PATH_TOOLS_H

#include <stdbool.h>
#include <stddef.h>

#define PATH_MAX_SIZE 4096

bool path_retrieve_basepath(char basepath[PATH_MAX_SIZE]);

#endif
