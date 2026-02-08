#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "../include/BiModalPredictor.hpp"
#include "../include/GSharePredictor.hpp"
#include "../include/HybridPredictor.hpp"

int main(int argc, char* argv[]) {
    // 1. Check if the user provided at least the type of predictor
    if (argc < 2) {
        std::cout << "Usage: ./simulator <type> <params...> <trace_file>" << std::endl;
        return 1;
    }

    std::string type = argv[1];
    Predictor* predictor = nullptr;
    std::string tracePath;
    std::string resultFileName;
    std::string commandEntered;

    for (int i = 0; i < argc; ++i) {
        commandEntered += argv[i];
        if (i < argc - 1) commandEntered += " "; // Add space between args
    }

    // 2. Setup the chosen predictor based on your command line input
    try {
        if (type == "bimodal") {
            // Command: ./simulator bimodal 1024 traces/gcc_trace.txt
            if (argc < 4) { 
                std::cerr << "Error: Bimodal needs [size] and [trace_file]" << std::endl; 
                return 1; 
            }
            int size = std::stoi(argv[2]);
            predictor = new BimodalPredictor(size);
            tracePath = argv[3];
            std::cout << "Predictor Type: Bimodal | Table Size: " << size << std::endl;
            resultFileName = "results_bimodal_" + std::string(argv[2]) + ".txt";
        } 
        else if (type == "gshare") {
            // Command: ./simulator gshare 8 4096 traces/gcc_trace.txt
            if (argc < 5) { 
                std::cerr << "Error: GShare needs [history_bits] [size] and [trace_file]" << std::endl; 
                return 1; 
            }
            int history = std::stoi(argv[2]);
            int size = std::stoi(argv[3]);
            predictor = new GSharePredictor(size, history);
            tracePath = argv[4];
            std::cout << "Predictor Type: GShare | History Bits: " << history << " | Table Size: " << size << std::endl;
            resultFileName = "results_gshare_h" + std::string(argv[2]) + "_s" + std::string(argv[3]) + ".txt";
        }
        else if (type == "hybrid") {
            // Command: ./simulator hybrid 1024 8 4096 1024 traces/gcc_trace.txt
            if (argc < 7) {
                std::cerr << "Error: Hybrid needs [bi_size] [gs_hist] [gs_size] [chooser_size] [trace]" << std::endl;
                return 1;
            }
            predictor = new HybridPredictor(std::stoi(argv[2]), std::stoi(argv[4]), std::stoi(argv[3]), std::stoi(argv[5]));
            tracePath = argv[6];
            std::cout << "Predictor Type: Hybrid (Tournament)" << std::endl;
            resultFileName = "results_hybrid.txt";
        }
    } catch (...) {
        std::cerr << "Error: Please ensure your parameters (like 1024 or 8) are numbers." << commandEntered << std::endl;
        return 1;
    }

    // 3. Open the trace file
    std::ifstream traceFile(tracePath);
    if (!traceFile.is_open()) {
        std::cerr << "Error: Could not open trace file at " << tracePath << std::endl;
        return 1;
    }

    // 4. Run the Simulation
    uint64_t addr;
    char outcome;
    long totalBranches = 0;
    long mispredictions = 0;

    while (traceFile >> std::hex >> addr >> outcome) {
        totalBranches++;
        // Handle both 'T'/'t' or 'N'/'n' (or even 1 and 0 if needed)
        bool actual = (outcome == 't' || outcome == 'T' || outcome == '1');

        bool prediction = predictor->predict(addr);
        if (prediction != actual) {
            mispredictions++;
        }
        
        predictor->update(addr, actual);
    }

    // 5. Final Output
    std::ofstream outFile(resultFileName);
    if (outFile.is_open()) {
        double missRate = ((double)mispredictions / totalBranches) * 100;
        
        // Write to file
        outFile << "COMMAND: " << commandEntered << std::endl;
        outFile << "Predictor Results: " << type << std::endl;
        outFile << "--------------------------------------------" << std::endl;
        outFile << "number of branches:                     " << totalBranches << std::endl;
        outFile << "number of mispredictions:               " << mispredictions << std::endl;
        outFile << std::fixed << std::setprecision(2);
        outFile << "misprediction rate:                     " << missRate << "%" << std::endl;
        outFile << "--------------------------------------------" << std::endl;
        
        outFile.close();
        std::cout << "Simulation complete. Results saved to: " << resultFileName << std::endl;
    } else {
        std::cerr << "Error: Could not create result file." << std::endl;
    }
    delete predictor;
    return 0;
}