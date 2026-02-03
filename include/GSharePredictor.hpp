#pragma once
#include "Predictor.hpp"
#include <vector>

class GSharePredictor : public Predictor {
private:
    std::vector<int> table;
    uint32_t globalHistory;
    uint32_t historyMask;
    int tableSize;
    int historyBits;

public:
    GSharePredictor(int numEntries, int bits) : tableSize(numEntries), historyBits(bits) {
        table.resize(tableSize, 1); // Initialize to Weakly Not Taken
        globalHistory = 0;          // Start with all NT
        historyMask = (1 << historyBits) - 1; // Creates a mask of '1's
    }

    bool predict(uint64_t address) override {
        // GShare Logic: Index = (PC ^ GlobalHistory) % tableSize
        uint32_t index = (address ^ globalHistory) % tableSize;
        return table[index] >= 2;
    }

    void update(uint64_t address, bool actual) override {
        uint32_t index = (address ^ globalHistory) % tableSize;
        
        // 1. Update the saturating counter
        if (actual) {
            if (table[index] < 3) table[index]++;
        } else {
            if (table[index] > 0) table[index]--;
        }

        // 2. Update Global History (Shift in the actual outcome)
        globalHistory = ((globalHistory << 1) | (actual ? 1 : 0)) & historyMask;
    }
};