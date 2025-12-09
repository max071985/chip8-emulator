#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

// Log levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void log_set_level(LogLevel level);
void log_msg(LogLevel level, const char* fmt, ...);

#endif
