#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#define LOG_MSG(msg) printf(msg); printf("\n");
//#define LOG_MSG(msg)

double get_time();



#endif
