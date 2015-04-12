//
//  distances.h
//  ABQ_cpp
//
//  Created by Elliot Meyerson on 4/11/15.
//  Copyright (c) 2015 Elliot Meyerson. All rights reserved.
//

#include <vector>

using std::vector;

namespace distances {
    
    double hamming (vector<double>&, vector<double>&);
    
    double manhattan (vector<double>&, vector<double>&);
    
    double euclidean (vector<double>&, vector<double>&);
    
};
