# Branch Prediction Simulator
This project is a C++ branch prediction simulator that models how modern CPUs predict branch instructions.

It supports:

* Bimodal branch predictor
* GShare branch predictor
* 2-bit saturating counters
* Offline simulation using branch trace files

## What This Simulator Does

* Reads a branch trace file (PC address + actual outcome)
* Predicts each branch as Taken or Not Taken
* Compares prediction with the real outcome
* Counts mispredictions
* Reports prediction accuracy

This is not a full CPU simulator because it focuses only on branch prediction.
