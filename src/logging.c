#include "logging.h"

int current_log_level = LOG_LEVEL_INFO;

void set_log_level(int level)
{
	if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_NONE)
		current_log_level = level;
}
