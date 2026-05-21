#pragma once

#include <iostream>

#define TRUFFLE_CHECK(condition)                                               \
    do {                                                                       \
        if (!(condition)) {                                                     \
            std::cerr << "check failed: " #condition << " at " << __FILE__    \
                      << ':' << __LINE__ << '\n';                               \
            return 1;                                                          \
        }                                                                       \
    } while (false)

