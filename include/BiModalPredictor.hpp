#pragma once
#include "Predictor.hpp"
#include <vector>

class BimodalPredictor : public Predictor {
private:
    std::vector<int> table;
    int tableSize;

public:
    BimodalPredictor(int numEntries) : tableSize(numEntries) {
        table.resize(tableSize, 1); // Initialize to Weakly Not Taken
    }

    bool predict(uint64_t address) override {
        int index = address % tableSize;
        return table[index] >= 2;
    }

    void update(uint64_t address, bool actual) override {
        int index = address % tableSize;
        if (actual) {
            if (table[index] < 3) table[index]++;
        } else {
            if (table[index] > 0) table[index]--;
        }
    }
};