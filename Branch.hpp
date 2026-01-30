#pragma once
#include <cstdint>

struct Branch {
    uint64_t address;      // Program counter address
    bool wasTaken;         // True if T(TAKEN), False if NT(NOT TAKEN)
};