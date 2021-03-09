// Created by Adam Pelc on 22/02/2021.
// Based on: https://github.com/rxi/log.c

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef WEBSERVER_C_PLAYGROUND_LOGGER_H
#define WEBSERVER_C_PLAYGROUND_LOGGER_H

#include <stdbool.h>

//! @brief Available logging levels.
typedef enum {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,
} LoggerLevel;

typedef enum {
  LOG_ENABLE,
  LOG_DISABLE,
} LoggerEnable;

#define log_trace(...) log_base(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_base(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_base(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warning(...) log_base(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_base(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_base(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

//! @brief Base method for logging data.
//! @param loggerLevel Sets global logger level.
//! @param fileName of file triggering logging.
//! @param fileLine of line in file triggering logging.
//! @param format output string.
//! @param ... format arguments.
void log_base(LoggerLevel loggerLevel, const char *fileName, int fileLine,
              const char *format, ...);

//! @brief Sets global logger level.
//! @param loggerLevel logger level.
void log_set_level(LoggerLevel loggerLevel);

//! @brief Sets logger
//! @param loggerEnable logger Enable/Disable.
void log_enable(LoggerEnable loggerEnable);

#endif

#pragma clang diagnostic pop
