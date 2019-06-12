#include "./string.h"

#include <stdlib.h>

size_t string_length(const char* str) {
    size_t i;
    for (i = 0; str[i] != '\0'; ++i);
    return i;
}

bool is_empty_string(const char* str) {
    return str[0] == '\0';
}

bool string_copy(char* dest, size_t max_dest_length, const char* src) {
    if (max_dest_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; i < max_dest_length; ++i) {
        dest[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    return src[i] == '\0';
}

bool string_equal(const char* str1, const char* str2) {
    size_t i;
    for (i = 0; str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'; ++i);
    return str1[i] == str2[i];
}

bool string_append(char* dest, size_t max_dest_length, const char* src) {
    if (is_empty_string(src)) {
        return true;
    }
    if (max_dest_length == 0) {
        return false;
    }

    size_t i, j;
    for (i = 0; dest[i] != '\0' && i < max_dest_length - 1; ++i);

    for (j = 0; src[j] != '\0' && i < max_dest_length - 1; ++j, ++i) {
        dest[i] = src[j];
    }
    dest[i] = '\0';

    return src[j] == '\0';
}

void string_reverse(char* str, size_t start_index, size_t end_index) {
    while (start_index < end_index) {
        char tmp = str[start_index];
        str[start_index] = str[end_index];
        str[end_index] = tmp;
        start_index++;
        end_index--;
    }
}

bool string_add_number_postfix(char* dest, size_t max_dest_length,
    const char* str, int num, int base)
{
    if (max_dest_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; str[i] != '\0' && i < max_dest_length - 1; ++i) {
        dest[i] = str[i];
    }
    if (i >= max_dest_length - 1) {
        dest[max_dest_length - 1] = '\0';
        return false;
    }

    if (num == 0) {
        dest[i] = '0';
        dest[i + 1] = '\0';
        return true;
    } else if (num < 0) {
        dest[i] = '-';
        num = -num;
        ++i;
    }

    size_t start_index = i;
    while (num != 0 && i < max_dest_length - 1) {
        div_t d = div(num, base);
        dest[i] = (d.rem > 9)? (d.rem - 10) + 'a' : d.rem + '0';
        num = d.quot;
        ++i;
    }

    dest[i] = '\0';
    string_reverse(dest, start_index, i - 1);

    return num == 0;
}
