#ifndef PATH_TOOLS_H
#define PATH_TOOLS_H

#include <stdbool.h>
#include <stddef.h>

#define PATH_MAX_SIZE 4096

bool path_retrieve_executable_filepath(char exe_filepath[PATH_MAX_SIZE]);
bool path_retrieve_executable_dir(char exe_directory[PATH_MAX_SIZE]);

#endif
