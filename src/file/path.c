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
    mem_free(bp);
    return true;
}
