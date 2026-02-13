# Branch Prediction Simulator

This project is a C++ branch prediction simulator that models how CPUs predict the direction of branch instructions. The simulator uses offline branch trace files containing program counter (PC) values and actual branch outcomes, then evaluates how accurately different branch prediction algorithms perform.

For each branch in the trace, the simulator makes a prediction (Taken or Not Taken), compares it with the actual outcome, updates the predictor state, and tracks mispredictions and overall accuracy. This project focuses only on branch prediction and does not simulate full CPU execution or pipelines.


## Index

1. [Architectural Concepts](#1-architectural-concepts)
2. [What This Simulator Does](#2-what-this-simulator-does)
3. [Implemented Branch Predictors](#3-implemented-branch-predictors)
4. [Running the Project (Windows)](#3-running-the-project-windows)



## 1. Architectural Concepts

### What is Branch Prediction?
In a CPU, instructions are processed in a pipeline - like an assembly line. To keep this line moving, the CPU needs to know which instructions come next. However, conditional branches like if statements or loops create a fork in the road!

The CPU doesn't want to wait until the if condition is acutally evaluated to find out where to go next because it would waste time/stall the pipeline. Instead, it guesses the outcome. If it guesses right, the CPU stays fast. However, a wrong prediction/guess means, it must throw away the speculative work and restart, which is expensive. 

* Taken (T / 1): The condition was true; the program jumps to a different memory address.

* Not Taken (NT / 0): The condition was false; the program continues to the next sequential instruction.

### What is a Branch Trace? 
A trace file is a log of a real program's execution. It tells us exactly what happened in the past so we can test our guesses against reality.

## 2. What This Simulator Does

* Reads a branch trace file (PC address + actual outcome)
* Predicts each branch as Taken or Not Taken
* Compares prediction with the real outcome
* Counts mispredictions
* Reports prediction accuracy

This is not a full CPU simulator because it focuses only on branch prediction.

## 3. Implemented Branch Predictors

### Bimodal Predictor
The bimodal predictor uses a table of 2-bit saturating counters indexed by the branch PC. Each counter tracks whether a branch is usually taken or not taken. This predictor is simple and serves as a baseline for comparison.

### GShare Predictor
GShare improves on the bimodal predictor by using a global history register. The global history is XORed with bits from the branch PC to index the prediction table, allowing the predictor to capture correlations between different branches (e.g., "If Branch A was taken, Branch B is usually not taken").

### Tournament (Hybrid) Predictor
Combines Bimodal and GShare. It uses a "Chooser" table of counters to learn which of the two predictors is more reliable for any given branch.

### Local History Predictor
The local history predictor maintains a separate history for each branch. A local history table records recent outcomes for individual branches, which is then used to index a pattern history table of 2-bit counters. This helps capture repeating patterns specific to a branch.

### GSelect Predictor
GSelect is similar to GShare but uses concatenation instead of XOR. Bits from the branch PC are combined with the global history register to form the index, allowing a comparison between different indexing strategies.

---

## 4. Project Roadmap (Upcoming Predictors)

### YAGS Predictor
YAGS (Yet Another Global Scheme) uses a bimodal predictor as a base and adds small exception caches for taken and not-taken branches. These caches correct cases where the bimodal predictor consistently makes mistakes, improving accuracy with modest complexity.

## 5. Running the Project (Windows)

### 1. Install Visual Studio Code

Download and install Visual Studio Code from the official documentation if you don't have it

- https://code.visualstudio.com/docs/setup/windows

After installation, open VS Code and install the **C/C++** extension by Microsoft from the Extensions tab.

---

### 2. Install a C++ Compiler (Windows)

To compile C++ on Windows, install **MinGW-w64** or **MSYS2** and ensure `g++` is added to your system `PATH`.

Official VS Code documentation for setting up C++ on Windows:

- https://code.visualstudio.com/docs/cpp/config-mingw

Verify the installation by running:

```bash
g++ --version
```
---

### 3. Build the Project

From the project root directory, compile the program using:

```bash
g++ -I include src/main.cpp -o simulator
```

---

**Bimodal Usage:** ./simulator bimodal <table_size> <trace_file>
```bash
./simulator bimodal 1024 traces/gcc_trace.txt
```

---

**GShare Usage:** ./simulator gshare <history_bits> <table_size> <trace_file>

**Arguments:**
1. History Bits ($m$): The number of past branch outcomes to remember.
2. Table Size: The number of entries (counters) available in the hardware.
3. Trace File: The path to your input data.



```bash
./simulator gshare 8 4096 traces/gcc_trace.txt
```

---

**Hybrid Usage:** ./simulator hybrid <bimodal_size> <gshare_history> <gshare_size> <chooser_size> <trace_file>

```bash
./simulator hybrid 1024 8 4096 1024 traces/gcc_trace.txt
```
---

**Local Predictor Usage:** ./simulator local <lht_size> <history_bits> <trace_file>

* **lht_size (e.g., 1024):** How many different branches can have their own unique diary.
* **history_bits (e.g., 10):** How many past events each branch remembers.

```bash
./simulator local 1024 10 traces/gcc_trace.txt
```

**GSelect Predictor:** ./simulator gselect <history_bits> <table_size> <trace_file>

```bash
./simulator gselect 8 4096 traces/gcc_trace.txt
```
---

For macOS users, compile using clang++:

```bash
clang++ -I include src/main.cpp -o simulator
```
