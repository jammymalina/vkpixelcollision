#include "./path.h"

#include "../logger/logger.h"
#include "../memory/memory.h"
#include "../string/string.h"

bool path_retrieve_basepath(char basepath[PATH_MAX_SIZE]) {
    string_copy(basepath, PATH_MAX_SIZE, "./");
    char* bp = SDL_GetBasePath();
    if (!bp) {
        return true;
    }
    size_t bp_len = string_length(bp);
    if (bp_len > PATH_MAX_SIZE - 1) {
        log_error("Base path is too long: %zu characters", bp_len);
        mem_free(bp);
        return false;
    }
    string_copy(basepath, PATH_MAX_SIZE, bp);
    bool retrieve_status = true;
    if (!string_ends_with(basepath, "/")) {
        retrieve_status = string_append(basepath, PATH_MAX_SIZE, "/");
    }
    mem_free(bp);
    return retrieve_status;
}

bool path_append_to_basepath(char dest[PATH_MAX_SIZE],
    const char basepath[PATH_MAX_SIZE], const char filepath[PATH_MAX_SIZE])
{
    string_copy(dest, PATH_MAX_SIZE, basepath);

    if (string_is_empty(filepath)) {
        return true;
    }

    const char* fp = filepath[0] == '/' ? &filepath[1] : filepath;

    char c;
    size_t i = string_length(dest);
    while ((c = *fp++) && i < PATH_MAX_SIZE - 1) {
        dest[i] = c;
        ++i;
    }
    dest[i] = '\0';

    return c == '\0';
}

bool path_extract_extension(const char path[PATH_MAX_SIZE],
    char extension[PATH_MAX_EXTENSION_SIZE])
{
    return path_extract_extension_nth(path, extension, 1);
}

bool path_extract_extension_nth(const char path[PATH_MAX_SIZE],
    char extension[PATH_MAX_EXTENSION_SIZE], size_t n)
{
    string_copy(extension, PATH_MAX_EXTENSION_SIZE, "");

    const ssize_t dot_idx = string_last_index_of_nth(path, '.', n);
    if (dot_idx == -1) {
        log_error("Unable to find extension");
        return false;
    }

    const ssize_t ext_end_index = n - 1 == 0 ?
        -1 : string_last_index_of_nth(path, '.', n - 1);
    bool extract_ext_status = string_substring_idx(extension,
        PATH_MAX_EXTENSION_SIZE, path, dot_idx + 1, ext_end_index);

    if (!extract_ext_status) {
        log_error("Unable to extract extension");
        return false;
    }

    return true;
}
