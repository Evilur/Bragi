#include "logger.h"

void Logger::Log(FILE* stream,
                const LogLevel log_level,
                const char* format, ...) {
    /* Get current time */
    char time_buffer[20];
    time_t current_time = time(nullptr);
    tm* time_info = localtime(&current_time);
    int offset = strftime(time_buffer, sizeof(time_buffer),
                          "%Y.%m.%d %H:%M:%S", time_info);

    /* Print the prefix */
    fprintf(stream, "%s[%s] %s",
            COLOR_CODE_STR[log_level],
            time_buffer,
            LOG_LEVEL_STR[log_level]);

    /* Print the message */
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    /* Print a new line char */
    fputc('\n', stream);
}
