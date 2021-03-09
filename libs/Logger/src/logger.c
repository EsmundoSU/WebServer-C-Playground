// Created by Adam Pelc on 22/02/2021.
// Based on: https://github.com/rxi/log.c
#include "../logger.h"
#include <stdarg.h>
#include <stdio.h>

static struct {
  LoggerLevel level;
  bool enable;
} configuration;

//! @brief Array of logger level strings.
//! @details Used for translate LoggerLevel to string.
static const char *logLevelString[] = {"TRACE",   "DEBUG", "INFO",
                                       "WARNING", "ERROR", "FATAL"};

void log_base(LoggerLevel loggerLevel, const char *fileName, int fileLine,
              const char *format, ...) {
  va_list args;

  if (configuration.level <= loggerLevel &&
      configuration.enable == LOG_ENABLE) {
    printf("%s:\t%s at line %i\n\t", logLevelString[loggerLevel], fileName,
           fileLine);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
  }
}

void log_set_level(LoggerLevel loggerLevel) {
  configuration.level = loggerLevel;
}

void log_enable(LoggerEnable enable) { configuration.enable = enable; }
