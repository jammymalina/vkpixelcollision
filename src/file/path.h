#ifndef PATH_TOOLS_H
#define PATH_TOOLS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define PATH_MAX_SIZE (FILENAME_MAX + 1)

bool path_retrieve_executable_filepath(char exe_filepath[PATH_MAX_SIZE]);
bool path_retrieve_executable_dir(char exe_directory[PATH_MAX_SIZE]);

#endif
