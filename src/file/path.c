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

bool path_extract_extension(const char path[PATH_MAX_SIZE],
    char extension[PATH_MAX_EXTENSION_SIZE])
{
    string_copy(extension, PATH_MAX_EXTENSION_SIZE, "");

    const ssize_t dot_idx = string_last_index_of(path, '.');
    if (dot_idx == -1) {
        log_error("Unable to find extension");
        return false;
    }

    bool extract_ext_status = string_substring_idx(extension,
        PATH_MAX_EXTENSION_SIZE, path, dot_idx + 1, -1);
    if (!extract_ext_status) {
        log_error("Unable to extract extension");
        return false;
    }

    return true;
}
