/*
 *  RecurrentNeuralNetwork.cpp
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include "RecurrentNetwork.h"
#include <iostream>

RecurrentNetwork::RecurrentNetwork() {
    
    int num_input = 16;
    int num_hidden = 2;
    int num_output = 3;
    
    input_end_ = num_input + 1;
    output_start_ = input_end_ + num_hidden;
    num_units_ = num_hidden + num_output;
    num_nodes_ = output_start_ + num_output;
    
    activation_.resize(num_nodes_);
    weights_.resize(num_nodes_);
    for (int i = 0; i < num_nodes_; i++) {
        weights_[i].resize(num_units_);
    }
}

void RecurrentNetwork::SetWeights(const vector<double> &genome) {
    for (int i = 0; i < genome.size(); i++) {
        //std::cout << genome[i];
        //std::cout << "\n";
        weights_[i/num_units_][i%num_units_] = genome[i];
    }
}

void RecurrentNetwork::SetInput (const vector<double> &input_values) {
    
    for (int i = 0; i < input_values.size(); i++) {
        activation_[i+1] = input_values[i];
    }
}

int RecurrentNetwork::GetAction () {
    
    int action = 0;
    double max_activation = -10.0;
    for (int i = output_start_; i < num_nodes_; i++) {
        if (activation_[i] > max_activation) {
            max_activation = activation_[i];
            action = i;
        }
    }
    return action - output_start_;
}

void RecurrentNetwork::Step () {
    
    vector<double> updated_activation(num_units_, 0.0);
    for (int i = 0; i < num_nodes_; i++) {
        for (int j = 0; j < num_units_; j++) {
            updated_activation[j] += activation_[i] * weights_[i][j];
        }
    }
    
    for (int i = 0; i < num_units_; i++) {
        activation_[i+input_end_] = tanh(updated_activation[i]);
    }
}

void RecurrentNetwork::Flush () {
    for (int i = input_end_; i < num_nodes_; i++) {
        activation_[i] = 0.0;
    }
}
