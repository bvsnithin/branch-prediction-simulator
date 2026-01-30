
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Branch.hpp"

using namespace std; 

int main(int argc, char* argv[]){

    // If arguments count is not 2, print an error message and provide the correct usage
    if(argc<2) {
        cerr << "Usage:" << argv[0] << " <trace_file>" << endl;
    }

    ifstream tracefile(argv[1]);

    if(!tracefile.is_open()){
        cerr << "Error opening the file" << endl;
    }

    uint64_t addr;
    string outcome;
    string line;
    long totalBranches = 0;
    long mispredictions = 0;

    while(getline(tracefile, line)){
        if (line.empty()) continue; // skip empty lines

        istringstream iss(line);
        if (!(iss >> hex >> addr >> outcome)) continue; // skip malformed lines

        cout << "Address: " << hex << addr << " Outcome: " << outcome << endl;

        totalBranches++;

        bool actual = (outcome=="T");

        bool prediction = true;

        if(prediction!=actual) {
            mispredictions++;
        }
    }

    double accuracy = (1.0 - (double)mispredictions / totalBranches) * 100;
    std::cout << "Total Branches: " << totalBranches << std::endl;
    std::cout << "Mispredictions: " << mispredictions << std::endl;
    std::cout << "Accuracy: " << accuracy << "%" << std::endl;
}