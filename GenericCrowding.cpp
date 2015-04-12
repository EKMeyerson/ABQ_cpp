/*
 *  GenericCrowding.cpp
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include "GenericCrowding.h"
#include <stdexcept>
#include <iostream>

GenericCrowding::GenericCrowding(Tartarus &task,
                                 std::string replaceSelect,
                                 std::string replaceBehavior,
                                 std::string replaceDistance) {
    
    task_ = task;
    
    // Load replace selection function
    if (replaceSelect == "CrowdingSelect") {
        ReplaceSelect = &GenericCrowding::CrowdingSelect;
    } else if (replaceSelect == "RandomSelect") {
        ReplaceSelect = &GenericCrowding::RandomSelect;
    } else if (replaceSelect == "WorstSelect") {
        ReplaceSelect = &GenericCrowding::WorstSelect;
    } else {
        throw std::invalid_argument( "Invalid replace select function." );
    }
    
    // Load replace behavior function
    if (replaceBehavior == "ActionHistoryBehavior") {
        UpdateBehavior = &GenericCrowding::UpdateBehaviorActionHistory;
    } else if (replaceBehavior == "FitnessBehavior") {
        UpdateBehavior = &GenericCrowding::UpdateBehaviorFitness;
    } else {
        throw std::invalid_argument( "Invalid replace behavior function." );
    }
    
    // Load replace distance function
    if (replaceDistance == "Hamming") {
        ReplaceDistance = distances::hamming;
    } else if (replaceDistance == "Manhattan") {
        ReplaceDistance = distances::manhattan;
    } else if (replaceDistance == "Euclidean") {
        ReplaceDistance = distances::euclidean;
    } else {
        throw std::invalid_argument( "Invalid replace distance function." );
    }
    

    num_iterations_ = 1000000;
    mutation_rate_ = 0.3;
    tournament_size_ = 10;
    population_size_ = 100;
    
    num_actuators_ = 1;
    num_sensors_ = 16;
    
    num_input_ = 16;
    num_hidden_ = 2;
    num_output_ = 3;
    num_nodes_ = 1+num_input_+num_hidden_+num_output_;
    
    genome_size_ = (num_nodes_)*(num_hidden_+num_output_);
    history_size_ = 80*100;
    
    curr_iteration_ = 0;
    
    total_fitness_ = 0;
    best_fitness_ = 0;
    
    population_.resize(population_size_+2);
    childA_ = population_size_;
    childB_ = population_size_ + 1;
    
    InitPopulation();
}

void GenericCrowding::InitPopulation() {
    
    for (long i = 0; i < population_size_; i++) {
        RandomIndividual(i);
        Evaluate(i);
        total_fitness_ += population_[i].fitness;
        if (population_[i].fitness > best_fitness_) {
            best_fitness_ = population_[i].fitness;
        }
    }
    RandomIndividual(childA_);
    RandomIndividual(childB_);
}

void GenericCrowding::RandomIndividual(long i) {
    
    population_[i].genome.resize(genome_size_);
    population_[i].history.resize(history_size_);
    for (long a = 0; a < history_size_; a++) {
        population_[i].history[a].resize(2);
        population_[i].history[a][0].resize(num_sensors_);
        population_[i].history[a][1].resize(num_sensors_);
    }
    for (long g = 0; g < genome_size_; g++) {
        population_[i].genome[g] = RandomWeight();
    }
}

void GenericCrowding::Next() {
    //std::cout << "TournamentSelecting..\n";
    long parentA = TournamentSelect();
    long parentB = TournamentSelect();
    //std::cout << "Crossover..\n";
    Crossover(parentA,parentB);
    //std::cout << "Mutating..\n";
    Mutate(childA_);
    Mutate(childB_);
    //std::cout << "Evaluating..\n";
    Evaluate(childA_);
    Evaluate(childB_);
    //std::cout << "CrowdSelecting..\n";
    long loserA = (this->*ReplaceSelect)(childA_);
    long loserB = (this->*ReplaceSelect)(childB_);
    //std::cout << "Replacing..\n";
    Replace(loserA, childA_);
    Replace(loserB, childB_);
    
    curr_iteration_++;
}

void GenericCrowding::Evaluate(long i) {

    task_.Reset();
    brain_.SetWeights(population_[i].genome);
    long action;
    vector<double> sensors;
    for (long a = 0; a < history_size_; a++) {
        if (a % 80 == 0) { brain_.Flush(); }
        sensors = task_.Sense();
        brain_.SetInput(sensors);
        brain_.Step();
        action = brain_.GetAction();
        //std::cout << " Action: ";
        //std::cout << action;
        //std::cout << "\n";
        task_.Act(action);
        RecordEvent(i,a,double(action),sensors);
    }
    population_[i].fitness = task_.fitness();
    (this->*UpdateBehavior)(i);
    //std::cout << task_.fitness();
    //std::cout << "\n";
}

long GenericCrowding::TournamentSelect() {
    
    long max_score = MIN_FITNESS;
    long winner = -1;
    for (long i = 0; i < tournament_size_; i++) {
        long j = rutil::pick_a_number(0,population_size_-1);
        if (population_[j].fitness > max_score) {
            max_score = population_[j].fitness;
            winner = j;
        }
    }
    return winner;
}

void GenericCrowding::Crossover(long parentA, long parentB) {

    long point = rutil::pick_a_number(0, genome_size_-1);
    for (long i = 0; i < point; i++) {
        population_[childA_].genome[i] = population_[parentA].genome[i];
        population_[childB_].genome[i] = population_[parentB].genome[i];
    }
    for (long i = point; i < genome_size_; i++) {
        population_[childA_].genome[i] = population_[parentB].genome[i];
        population_[childB_].genome[i] = population_[parentA].genome[i];
    }
}

void GenericCrowding::Mutate(long i) {
    
    for (long g = 0; g < genome_size_; g++) {
        if (rutil::pick_a_number(0.0, 1.0) < mutation_rate_){
            population_[i].genome[g] = RandomWeight();
        }
    }
}

long GenericCrowding::CrowdingSelect(long child) {
    
    double min_distance = MAX_DISTANCE;
    long loser = -1;
    for (long i = 0; i < tournament_size_; i++) {
        long j = rutil::pick_a_number(0, population_size_-1);
        double dist = ReplaceDistance(population_[child].behavior, population_[j].behavior);
        if (dist < min_distance) {
            min_distance = dist;
            loser = j;
        }
    }
    //std::cout << "Loser: ";
    //std::cout << loser;
    //std::cout << "\n";
    return loser;
}

long GenericCrowding::RandomSelect(long child) {
    return rutil::pick_a_number(0, population_size_-1);
}

long GenericCrowding::WorstSelect(long child) {
    
    double min_fitness = MAX_FITNESS;
    long loser = -1;
    for (long i = 0; i < population_size_; i++) {
        if (population_[i].fitness < min_fitness) {
            min_fitness = population_[i].fitness;
            loser = i;
        }
    }
    return loser;
}

void GenericCrowding::Replace(long loser, long winner) {
    if (population_[winner].fitness > best_fitness_) {
        best_fitness_ = population_[winner].fitness;
    }
    total_fitness_ -= population_[loser].fitness;
    total_fitness_ += population_[winner].fitness;
    /**
    std::cout << '\n';
    std::cout << "Loser genome: " << population_[loser].action_history[0] << '\n';
    std::cout << "Winner genome: " << population_[winner].action_history[0] << '\n';
    **/
     std::swap(population_[loser],population_[winner]);
    /**
    std::cout << "Loser genome: " << population_[loser].action_history[0] << '\n';
    std::cout << "Winner genome: " << population_[winner].action_history[0] << '\n';
    population_[winner].action_history[0] = 1;
    std::cout << "Loser genome: " << population_[loser].action_history[0] << '\n';
    std::cout << "Winner genome: " << population_[winner].action_history[0] << '\n';
    **/
    
    
     //Individual *tmp = population_[loser];
    //std::cout << &tmp << '\n';
    //tmp.fitness = 12;
    //std::cout << population_[loser].fitness << '\n';
    //std::cout << &population_[loser] << '\n' << &population_[winner] << '\n';
    //population_[loser] = population_[winner];
    //population_[winner] = tmp;
    //std::cout << &population_[loser] << '\n' << &population_[winner] << '\n';
}

void GenericCrowding::RecordEvent(long i, long a, double action, vector<double> sensors) {
    
    //std::cout << '\n';
    //std::cout << population_[i].history.size() << '\n';
    //std::cout << population_[i].history[0].size() << '\n';
    //std::cout << population_[i].history[a].size() << '\n';
    //std::cout << population_[i].history[a][0].size() << '\n';
    //std::cout << population_[i].history[a][0][0] << '\n';
    population_[i].history[a][0][0] = action;
    for (long j = 0; j < sensors.size(); j++) {
        population_[i].history[a][1][j] = sensors[j];
    }
}

double GenericCrowding::RandomWeight() {
    
    double weight = rutil::pick_a_number(-10.0, 10.0);//(dis_(gen_)*20)-10;
    //std::cout << weight;
    //std::cout << "\n";
    return weight;
}

bool GenericCrowding::Done() { return curr_iteration_ == num_iterations_; }

double GenericCrowding::best_fitness() { return best_fitness_; }

double GenericCrowding::avg_fitness() { return total_fitness_ / population_size_; }

void GenericCrowding::UpdateBehaviorActionHistory(long i) {
    
    if (population_[i].behavior.size() != history_size_) {
        population_[i].behavior.resize(history_size_);
    }
    
    for (long a = 0; a < history_size_; a++) {
        //std::cout << '\n';
        //std::cout << population_[i].history[a][0].size() << '\n';
        //std::cout << population_[i].history[a][0][0] << '\n';
        //std::cout << population_[i].behavior.size() << '\n';
        population_[i].behavior[a] = population_[i].history[a][0][0];
    }
}

void GenericCrowding::UpdateBehaviorFitness(long i) {
    
    if (population_[i].behavior.size() != 1) {
        population_[i].behavior.push_back(population_[i].fitness);
    } else {
        population_[i].behavior[0] = population_[i].fitness;
    }

}


/**
long GenericCrowding::HammingDistance(long i, long j) {
    
    long dist = 0;
    for (long a = 0; a < history_size_; a++) {
        if (population_[i].history[a] != population_[j].history[a]) {
            dist++;
        }
    }
    return dist;
}
**/



