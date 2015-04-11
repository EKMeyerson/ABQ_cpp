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
    vector<double> action_history;
    vector<double> behavior;
    double fitness;
};


class GenericCrowding {
    
    static const long MIN_FITNESS = -1000000;
    static const long MAX_DISTANCE = 1000000;
    
    // Configuration parameters
    long num_iterations_;
    double mutation_rate_;
    long tournament_size_;
    long population_size_;
    long genome_size_;
    long action_history_size_;
    //double (*SelectDist) (vector<double>&,vector<double>&);
    double (*ReplaceDist) (vector<double>&,vector<double>&);
    
    // State information
    Tartarus task_;
    vector<Individual> population_;
    RecurrentNetwork brain_;
    long curr_iteration_;
    double total_fitness_;
    double best_fitness_;
    long childA_;
    long childB_;
    
    // Evolution methods (long params refer to Indivs index in population)
    void InitPopulation();
    void RandomIndividual(long);
    long TournamentSelect();
    void Crossover(long,long);
    void Mutate(long);
    void Evaluate(long);
    void (*UpdateBehavior) (long);
    long (*ReplaceSelect) (long);
    void Replace(long,long);
    double RandomWeight();
    long HammingDistance(long,long);
    
    // Options for parent and replacement selection
    long CrowdingSelect(long);
    long RandomSelect (long);
    long WorstSelect (long);
    
    // Options for behavior quantification
    void UpdateBehaviorActionHistory (long);
    void UpdateBehaviorFitness (long);
    void UpdateBehaviorEventCounts (long);
    
    
public:
    GenericCrowding(Tartarus&, double (*) (vector<double>&,vector<double>&));
    void Next();
    double best_fitness();
    double avg_fitness();
    bool Done();
    
};