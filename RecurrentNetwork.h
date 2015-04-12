/*
 *  RecurrentNeuralNetwork.h
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include <vector>
#include <cmath>

using std::vector;

class RecurrentNetwork {
    
    long input_end_, output_start_, num_units_, num_nodes_;
    vector< vector<double> > weights_;
    vector<double> activation_;
    
public:
    RecurrentNetwork ();
    void SetWeights (const vector<double> &genome);
    void SetInput (const vector<double>&);
    long GetAction ();
    void Step ();
    void Flush ();
};
