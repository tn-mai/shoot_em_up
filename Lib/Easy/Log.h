/**
* @file Log.h
*/
#ifndef EASY_LOG_H_INCLUDED
#define EASY_LOG_H_INCLUDED
#include <stdio.h>
#include <Windows.h>

#define LOG(...) { \
  char log_buf__[1024]; \
  snprintf(log_buf__, 1024, __VA_ARGS__); \
  fprintf(stderr, log_buf__); \
  OutputDebugStringA(log_buf__); \
} (void)0

#endif // EASY_LOG_H_INCLUDED