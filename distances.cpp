//
//  distances.cpp
//  ABQ_cpp
//
//  Created by Elliot Meyerson on 4/11/15.
//  Copyright (c) 2015 Elliot Meyerson. All rights reserved.
//

#include <cmath>

#include "distances.h"


namespace distances {
    
    double hamming (vector<double> & b1, vector<double> & b2) {
        
        long dist = 0;
        for (long i = 0; i < b1.size(); i++) {
            if (std::abs(b1[i] - b2[i]) > 0.1) {
                dist++;
            }
        }
        return double( dist );
    }
    
    double manhattan (vector<double> & b1, vector<double> & b2) {
        
        double dist = 0;
        for (long i = 0; i < b1.size(); i++) {
            dist += std::abs(b1[i] - b2[i]);
        }
        return dist;
    }
    
    double euclidean (vector<double> & b1, vector<double> & b2) {
        
        double dist = 0;
        for (long i = 0; i < b1.size(); i++) {
            dist += std::pow(std::abs(b1[i] - b2[i]), 2);
        }
        return std::sqrt( dist );
    }
    
}