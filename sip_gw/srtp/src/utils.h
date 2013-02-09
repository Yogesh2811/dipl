#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif


double get_time();

#endif
