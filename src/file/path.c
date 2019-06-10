#include "./path.h"

#include <unistd.h>
#include <libgen.h>

#include "../logger/logger.h"
#include "../string/string.h"

bool path_retrieve_executable_filepath(char exe_filepath[PATH_MAX_SIZE]) {
    string_copy(exe_filepath, PATH_MAX_SIZE, "");
    ssize_t len = readlink("/proc/self/exe", exe_filepath, PATH_MAX_SIZE - 1);
    if (len == -1) {
        log_error("Unable to retrieve executable filepath");
        return false;
    }
    exe_filepath[len] = '\0';
    return true;
}

bool path_retrieve_executable_dir(char exe_directory[PATH_MAX_SIZE]) {
    string_copy(exe_directory, PATH_MAX_SIZE, "");
    char exe_filepath[PATH_MAX_SIZE];
    if (!path_retrieve_executable_filepath(exe_filepath)) {
        log_error("Unable to retrieve executable directory");
        return false;
    }
    const char* d = dirname(exe_filepath);
    if (!string_copy(exe_directory, PATH_MAX_SIZE, d)) {
        log_error("Unable to retrieve executable directory, copy failed");
        return false;
    }
    return true;
}
