/*
 *  GenericCrowding.cpp
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include "GenericCrowding.h"
#include <iostream>
#include "rutil.h"


GenericCrowding::GenericCrowding(Tartarus &task, double (*distanceFunc) (vector<double>&,vector<double>&)) {
    
    task_ = task;
    ReplaceDist = distanceFunc;
    
    num_iterations_ = 1000000;
    mutation_rate_ = 0.3;
    tournament_size_ = 10;
    population_size_ = 100;
    
    genome_size_ = (24+1+10)*10;
    action_history_size_ = 80*100;
    
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
    population_[i].action_history.resize(action_history_size_);
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
    long loserA = CrowdingSelect(childA_);
    long loserB = CrowdingSelect(childB_);
    //std::cout << "Replacing..\n";
    Replace(loserA, childA_);
    Replace(loserB, childB_);
    
    curr_iteration_++;
}

void GenericCrowding::Evaluate(long i) {

    task_.Reset();
    brain_.SetWeights(population_[i].genome);
    long action;
    for (long a = 0; a < action_history_size_; a++) {
        if (a % 80 == 0) { brain_.Flush(); }
        brain_.SetInput(task_.Sense());
        brain_.Step();
        action = brain_.GetAction();
        //std::cout << " Action: ";
        //std::cout << action;
        //std::cout << "\n";
        task_.Act(action);
        population_[i].action_history[a] = action;
    }
    population_[i].fitness = task_.fitness();
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
        double dist = ReplaceDist(population_[child].action_history, population_[j].action_history);
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

double GenericCrowding::RandomWeight() {
    
    double weight = rutil::pick_a_number(-10.0, 10.0);//(dis_(gen_)*20)-10;
    //std::cout << weight;
    //std::cout << "\n";
    return weight;
}

bool GenericCrowding::Done() { return curr_iteration_ == num_iterations_; }

double GenericCrowding::best_fitness() { return best_fitness_; }

double GenericCrowding::avg_fitness() { return total_fitness_ / population_size_; }

long GenericCrowding::HammingDistance(long i, long j) {
    
    long dist = 0;
    for (long a = 0; a < action_history_size_; a++) {
        if (population_[i].action_history[a] != population_[j].action_history[a]) {
            dist++;
        }
    }
    return dist;
}



