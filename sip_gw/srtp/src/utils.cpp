#include "utils.h"

double get_time(void)
{
#if _WIN32
    static int initialized = 0;
    static LARGE_INTEGER frequency;
    LARGE_INTEGER value;

    if (!initialized) {
        initialized = 1;
    }

    QueryPerformanceCounter(&value);
    return (double)value.QuadPart / (double)frequency.QuadPart;
#else
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == -1) {
        //cerr << "time error\n";
    }
    return (double)tv.tv_sec + (double)tv.tv_usec/1000000.;
#endif
}
