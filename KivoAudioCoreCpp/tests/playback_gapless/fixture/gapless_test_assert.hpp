#pragma once

#include <stdexcept>

#define GAPLESS_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (false)
