/*
 *  ReplicaExperiment1.cpp
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include "GenericCrowding.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "rutil.h"
#include "distances.h"

int main(int argc, const char * argv[]) {
    
    rutil::randomize();
    Tartarus task{};
    
    // Load command line arguments
    if (argc != 5) { throw std::invalid_argument ("Not enough command line args"); }
    
    std::string outfileName = argv[1];
    
    std::string replaceSelect = argv[2];
    std::string replaceBehavior = argv[3];
    std::string replaceDistance = argv[4];

    std::cout << "\nOutfile: " << outfileName;
    std::cout << "\nReplace Select Function: " << replaceSelect;
    std::cout << "\nReplace Behavior: " << replaceBehavior;
    std::cout << "\nReplace Behavior Distance Function: " << replaceDistance << '\n';
    
    //double (*dist) (vector<double>&,vector<double>&) = distances::hamming;
    GenericCrowding ne(task,replaceSelect,replaceBehavior,replaceDistance);
    
    std::fstream outfile;
    
    for (int i = 0; i < 1000000; i++) {
        if (i % 100 == 0) {
            printf ("%d %f %f \n",i,ne.best_fitness(),ne.avg_fitness());
            outfile.open(outfileName, std::fstream::app | std::fstream::out);
            outfile << i << ' ' << ne.best_fitness() << ' ' << ne.avg_fitness() << "\n";
            outfile.close();
        }
        ne.Next();
    }
    std::cout << "Done.";
    return 0;
}

