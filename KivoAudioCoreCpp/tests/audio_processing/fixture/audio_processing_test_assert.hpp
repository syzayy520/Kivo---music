#pragma once

#include <stdexcept>

#define AUDIO_PROCESSING_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error(#condition); \
        } \
    } while (false)
