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
#include <stdio.h>
#include "rutil.h"

int main(int argc, const char * argv[]) {
    
    rutil::randomize();
    Tartarus task{};
    GenericCrowding ne(task);
    
    for (int i = 0; i < 1000000; i++) {
        if (i % 100 == 0) {
        printf ("%d %f %f \n",i,ne.best_fitness(),ne.avg_fitness());
        }
        ne.Next();
    }
    std::cout << "Done.";
    return 0;
}

