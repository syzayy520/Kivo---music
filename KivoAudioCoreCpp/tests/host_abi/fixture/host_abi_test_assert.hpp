#pragma once

#include <cstdlib>
#include <iostream>

#define HOST_ABI_ASSERT(condition)                                        \
    do {                                                                  \
        if (!(condition)) {                                               \
            std::cerr << "Host ABI assertion failed: " #condition         \
                      << " at " << __FILE__ << ':' << __LINE__ << '\n';   \
            std::abort();                                                 \
        }                                                                 \
    } while (false)
