#pragma once

#include <cstdint>
#include <stdlib.h>
#include <sys/time.h>

namespace proxybench {

class Times
{
public:
    static int64_t current_millis(void)
    {
        struct timeval result;
        gettimeofday(&result, NULL);
        return result.tv_sec * 1000 + result.tv_usec / 1000;
    };

public:
    Times() = delete;
};

} // namespace proxybench {
