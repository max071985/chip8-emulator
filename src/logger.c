#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

/*
    logger.c outputs log msgs based on current logging level (current_level)
    - logging levels are declared in logger.h
*/

static LogLevel current_level = LOG_INFO;

void log_set_level(LogLevel level) {
    current_level = level;
}

static const char* level_to_string(LogLevel level) {
    switch (level) {
        case LOG_DEBUG: return "[DEBUG]";
        case LOG_INFO:  return "[INFO]";
        case LOG_WARN:  return "[WARN]";
        case LOG_ERROR: return "[ERROR]";
        default:        return "[UNKWN]";
    }
}

/*
    Writes to stderr if the error is appropriate to the declared logging level.
*/
void log_msg(LogLevel level, const char* fmt, ...) {
    if (level < current_level) return;  // respect threshold

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s ", level_to_string(level));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}
