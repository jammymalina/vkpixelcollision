#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <sys/types.h>

ssize_t file_retrieve_byte_size(const char* filename);
ssize_t file_read_binary_file(const char* filename, char** data);

#endif
