#pragma once
#include <cstdint>

class Predictor {
public:
    // Virtual destructor is important for C++ memory safety
    virtual ~Predictor() {}

    // Every predictor must implement these two functions
    virtual bool predict(uint64_t address) = 0;
    virtual void update(uint64_t address, bool actual) = 0;
};