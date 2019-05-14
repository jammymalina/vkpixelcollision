#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>

size_t string_length(const char *str);
bool is_empty_string(const char *str);
bool string_copy(char *dest, size_t max_dest_length, const char *src);
bool string_equal(const char *str1, const char *str2);
bool string_append(char *dest, size_t max_dest_length, const char *src);
void string_reverse(char *str, size_t start_index, size_t end_index);
bool string_add_number_postfix(char *dest, size_t max_dest_length, const char *str, int num, int base);

#endif
