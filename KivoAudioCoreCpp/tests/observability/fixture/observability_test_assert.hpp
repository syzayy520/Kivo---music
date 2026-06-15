#pragma once

#include <cstdlib>
#include <iostream>

#define OBSERVABILITY_ASSERT(condition)                                  \
    do {                                                                  \
        if (!(condition)) {                                               \
            std::cerr << "Observability assertion failed: " #condition    \
                      << " at " << __FILE__ << ':' << __LINE__ << '\n';   \
            std::abort();                                                 \
        }                                                                 \
    } while (false)
