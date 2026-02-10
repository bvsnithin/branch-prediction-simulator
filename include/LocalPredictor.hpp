#pragma once
#include "Predictor.hpp"
#include <vector>

class LocalPredictor : public Predictor {
private:
    std::vector<uint32_t> localHistoryTable; // LHT: Stores history per branch
    std::vector<int> patternHistoryTable;    // PHT: Stores 2-bit counters
    
    int lhtSize;        // Number of rows in LHT
    int historyBits;    // How many bits of history per branch (e.g., 10 bits)
    int phtSize;        // Number of counters in PHT (usually 2^historyBits)
    uint32_t historyMask;

public:
    LocalPredictor(int lhtEntries, int bits) : lhtSize(lhtEntries), historyBits(bits) {
        // Initialize LHT with 0 (Not Taken)
        localHistoryTable.resize(lhtSize, 0);
        
        // PHT size is determined by history bits (e.g., 10 bits = 1024 counters)
        phtSize = (1 << historyBits); 
        patternHistoryTable.resize(phtSize, 1); // Initialize to Weakly Not Taken
        
        historyMask = (1 << historyBits) - 1;
    }

    bool predict(uint64_t address) override {
        // Step 1: Find this branch's local history
        int lhtIndex = address % lhtSize;
        uint32_t history = localHistoryTable[lhtIndex];
        
        // Step 2: Use that history to find the prediction in PHT
        int phtIndex = history & historyMask;
        return patternHistoryTable[phtIndex] >= 2;
    }

    void update(uint64_t address, bool actual) override {
        int lhtIndex = address % lhtSize;
        uint32_t history = localHistoryTable[lhtIndex];
        int phtIndex = history & historyMask;

        // 1. Update the PHT counter based on actual outcome
        if (actual) {
            if (patternHistoryTable[phtIndex] < 3) patternHistoryTable[phtIndex]++;
        } else {
            if (patternHistoryTable[phtIndex] > 0) patternHistoryTable[phtIndex]--;
        }

        // 2. Update the Local History (shift in the new result)
        localHistoryTable[lhtIndex] = ((history << 1) | (actual ? 1 : 0)) & historyMask;
    }
};