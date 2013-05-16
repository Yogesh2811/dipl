/** @file */
/**
 *  Various utilities for the implementation 
 */
 
#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

/** \brief Logger of messages in implementation*/
#define LOG_MSG(...) printf("LOG "); printf(__VA_ARGS__); printf("\n");
/** \brief Logger of errors in implementation*/
#define LOG_ERROR(...) printf("ERR "); printf(__VA_ARGS__); printf("\n");
//#define LOG_MSG(msg) //turn off logging

/** \brief time measurement*/
double get_time();

#endif
/** @} */