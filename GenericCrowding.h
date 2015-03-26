/*
 *  GenericCrowding.h
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include <vector>
#include <stdlib.h>
#include "RecurrentNetwork.h"
#include "Tartarus.h"
//#include <random>

using std::vector;


struct Individual {
    vector<double> genome;
    vector<int> action_history;
    vector<int> behavior;
    int fitness;
};


class GenericCrowding {
    
    static const int MIN_FITNESS = -1000000;
    static const int MAX_DISTANCE = 1000000;
    
    // Configuration parameters
    int num_iterations_;
    double mutation_rate_;
    int tournament_size_;
    int population_size_;
    int genome_size_;
    int action_history_size_;
    
    // State information
    Tartarus task_;
    vector<Individual> population_;
    RecurrentNetwork brain_;
    int curr_iteration_;
    double total_fitness_;
    double best_fitness_;
    int childA_;
    int childB_;
    
    // Evolution methods
    void InitPopulation();
    void RandomIndividual(int);
    int TournamentSelect();
    void Crossover(int,int);
    void Mutate(int);
    void Evaluate(int);
    int CrowdingSelect(int);
    void Replace(int,int);
    double RandomWeight();
    int HammingDistance(int,int);
    
public:
    GenericCrowding(Tartarus&);
    void Next();
    double best_fitness();
    double avg_fitness();
    bool Done();
    
};