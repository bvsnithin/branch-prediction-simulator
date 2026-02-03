#pragma once
#include "BiModalPredictor.hpp"
#include "GSharePredictor.hpp"
#include "Predictor.hpp"
#include <vector>

class HybridPredictor : public Predictor {
private:
  BimodalPredictor *bimodal;
  GSharePredictor *gshare;
  std::vector<int> chooserTable;
  int chooserSize;

public:
  HybridPredictor(int bimodalSize, int gshareSize, int gshareHistory,
                  int chooserEntries) {
    bimodal = new BimodalPredictor(bimodalSize);
    gshare = new GSharePredictor(gshareSize, gshareHistory);
    chooserSize = chooserEntries;
    // Start at 1 (Weakly prefer Bimodal)
    chooserTable.resize(chooserSize, 1);
  }

  ~HybridPredictor() {
    delete bimodal;
    delete gshare;
  }

  bool predict(uint64_t address) override {
    int index = address % chooserSize;
    if (chooserTable[index] >= 2) {
      return gshare->predict(address);
    } else {
      return bimodal->predict(address);
    }
  }

  void update(uint64_t address, bool actual) override {
    int index = address % chooserSize;
    bool bimodalPred = bimodal->predict(address);
    bool gsharePred = gshare->predict(address);

    // Update the Chooser only if the predictions were different
    if (bimodalPred != gsharePred) {
      if (gsharePred == actual) {
        // GShare was right!
        if (chooserTable[index] < 3)
          chooserTable[index]++;
      } else if (bimodalPred == actual) {
        // Bimodal was right!
        if (chooserTable[index] > 0)
          chooserTable[index]--;
      }
    }

    // Always update the underlying predictors
    bimodal->update(address, actual);
    gshare->update(address, actual);
  }
};