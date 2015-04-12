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
#include "distances.h"
//#include <random>

using std::vector;


struct Individual {
    vector<double> genome;
    vector< vector< vector<double> > > history;
    vector<double> behavior;
    double fitness;
};


class GenericCrowding {
    
    static const long MIN_FITNESS = -1000000;
    static const long MAX_DISTANCE = 1000000;
    static const long MAX_FITNESS = 1000000;
    
    // Configuration parameters
    long num_iterations_;
    double mutation_rate_;
    long tournament_size_;
    long population_size_;
    long num_actuators_;
    long num_sensors_;
    long num_input_;
    long num_hidden_;
    long num_output_;
    long num_nodes_;
    long genome_size_;
    long history_size_;
    //double (*SelectDist) (vector<double>&,vector<double>&);

    
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
    void RecordEvent(long,long,double,vector<double>);
    void (GenericCrowding::*UpdateBehavior) (long);
    double (*ReplaceDistance) (vector<double>&,vector<double>&);
    long (GenericCrowding::*ReplaceSelect) (long);
    void Replace(long,long);
    double RandomWeight();
    //long HammingDistance(long,long);
    
    // Options for parent and replacement selection
    long CrowdingSelect(long);
    long RandomSelect (long);
    long WorstSelect (long);
    
    // Options for behavior quantification
    void UpdateBehaviorActionHistory (long);
    void UpdateBehaviorFitness (long);
    void UpdateBehaviorEventCounts (long);
    
    
public:
    GenericCrowding(Tartarus&,
                    std::string,
                    std::string,
                    std::string);
    void Next();
    double best_fitness();
    double avg_fitness();
    bool Done();
    
};