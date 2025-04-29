#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// Log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_NONE 2

extern int current_log_level;

void set_log_level(int level);

// Logging macros
#define LOG(level, tag, msg, ...) \
	do { if ((level) >= current_log_level) fprintf(stderr, "[" tag "] " msg "\n", ##__VA_ARGS__); } while (0)

#define LOG_DEBUG(msg, ...) LOG(LOG_LEVEL_DEBUG, "DEBUG", msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) LOG(LOG_LEVEL_INFO, "INFO", msg, ##__VA_ARGS__)

#endif
