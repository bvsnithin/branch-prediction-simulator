#pragma once
#include "Predictor.hpp"
#include <vector>

class GSelectPredictor : public Predictor {
private:
    std::vector<int> table;
    uint32_t globalHistory;
    uint32_t historyMask;
    int tableSize;
    int historyBits;

public:
    GSelectPredictor(int numEntries, int bits) : tableSize(numEntries), historyBits(bits) {
        table.resize(tableSize, 1); // Initialize to Weakly Not Taken
        globalHistory = 0;
        historyMask = (1 << historyBits) - 1;
    }

    bool predict(uint64_t address) override {
        // GSelect Logic: Concatenate PC bits and History bits
        // We take the lower bits of the PC and shift them to make room for history
        // Index = (PC_bits << historyBits) | GlobalHistory
        
        // Let's calculate how many bits of the PC we can use based on table size
        // If table is 4096 (2^12) and history is 4 bits, we use 8 bits of PC.
        uint32_t index = (address << historyBits) | (globalHistory & historyMask);
        index = index % tableSize; 
        
        return table[index] >= 2;
    }

    void update(uint64_t address, bool actual) override {
        uint32_t index = (address << historyBits) | (globalHistory & historyMask);
        index = index % tableSize;

        if (actual) {
            if (table[index] < 3) table[index]++;
        } else {
            if (table[index] > 0) table[index]--;
        }

        // Update Global History
        globalHistory = ((globalHistory << 1) | (actual ? 1 : 0)) & historyMask;
    }
};