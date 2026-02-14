#pragma once
#include "Predictor.hpp"
#include <vector>

struct CacheEntry {
    int counter;
    uint32_t tag;
};

class YAGSPredictor : public Predictor {
private:
    std::vector<int> bimodalTable;
    std::vector<CacheEntry> takenCache;
    std::vector<CacheEntry> notTakenCache;
    
    uint32_t globalHistory;
    uint32_t historyMask;
    int bimodalSize;
    int cacheSize;

public:
    YAGSPredictor(int biSize, int cSize, int hBits) {
        bimodalSize = biSize;
        cacheSize = cSize;
        bimodalTable.resize(bimodalSize, 1); // Weakly NT
        
        CacheEntry empty = {1, 0}; // Weakly NT, Tag 0
        takenCache.resize(cacheSize, empty);
        notTakenCache.resize(cacheSize, empty);
        
        globalHistory = 0;
        historyMask = (1 << hBits) - 1;
    }

    bool predict(uint64_t address) override {
        int biIndex = address % bimodalSize;
        bool biPrediction = (bimodalTable[biIndex] >= 2);
        
        uint32_t cacheIndex = (address ^ globalHistory) % cacheSize;
        uint32_t tag = (address >> 10) & 0xFF; // Simple tag from upper bits

        if (biPrediction) { // Bimodal says Taken, check Not-Taken Cache for exceptions
            if (notTakenCache[cacheIndex].tag == tag) {
                return notTakenCache[cacheIndex].counter >= 2;
            }
            return true;
        } else { // Bimodal says Not-Taken, check Taken Cache for exceptions
            if (takenCache[cacheIndex].tag == tag) {
                return takenCache[cacheIndex].counter >= 2;
            }
            return false;
        }
    }

    void update(uint64_t address, bool actual) override {
        int biIndex = address % bimodalSize;
        bool biPrediction = (bimodalTable[biIndex] >= 2);
        uint32_t cacheIndex = (address ^ globalHistory) % cacheSize;
        uint32_t tag = (address >> 10) & 0xFF;

        // 1. Update the Exception Caches ONLY if necessary
        if (biPrediction) { // Bimodal said Taken
            if (actual == false || notTakenCache[cacheIndex].tag == tag) {
                updateCounter(notTakenCache[cacheIndex].counter, actual);
                notTakenCache[cacheIndex].tag = tag;
            }
        } else { // Bimodal said Not-Taken
            if (actual == true || takenCache[cacheIndex].tag == tag) {
                updateCounter(takenCache[cacheIndex].counter, actual);
                takenCache[cacheIndex].tag = tag;
            }
        }

        // 2. Always update the Bimodal (Base) table
        updateCounter(bimodalTable[biIndex], actual);

        // 3. Update Global History
        globalHistory = ((globalHistory << 1) | (actual ? 1 : 0)) & historyMask;
    }

private:
    void updateCounter(int &counter, bool actual) {
        if (actual && counter < 3) counter++;
        else if (!actual && counter > 0) counter--;
    }
};