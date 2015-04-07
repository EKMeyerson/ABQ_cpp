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


GenericCrowding::GenericCrowding(Tartarus &task) {
    
    task_ = task;
    
    num_iterations_ = 1000000;
    mutation_rate_ = 0.3;
    tournament_size_ = 10;
    population_size_ = 100;
    
    genome_size_ = (16+1+5)*5;
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
    
    for (int i = 0; i < population_size_; i++) {
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

void GenericCrowding::RandomIndividual(int i) {
    
    population_[i].genome.resize(genome_size_);
    population_[i].action_history.resize(action_history_size_);
    for (int g = 0; g < genome_size_; g++) {
        population_[i].genome[g] = RandomWeight();
    }
}

void GenericCrowding::Next() {
    //std::cout << "TournamentSelecting..\n";
    int parentA = TournamentSelect();
    int parentB = TournamentSelect();
    //std::cout << "Crossover..\n";
    Crossover(parentA,parentB);
    //std::cout << "Mutating..\n";
    Mutate(childA_);
    Mutate(childB_);
    //std::cout << "Evaluating..\n";
    Evaluate(childA_);
    Evaluate(childB_);
    //std::cout << "CrowdSelecting..\n";
    int loserA = CrowdingSelect(childA_);
    int loserB = CrowdingSelect(childB_);
    //std::cout << "Replacing..\n";
    Replace(loserA, childA_);
    Replace(loserB, childB_);
    
    curr_iteration_++;
}

void GenericCrowding::Evaluate(int i) {

    task_.Reset();
    brain_.SetWeights(population_[i].genome);
    int action;
    for (int a = 0; a < action_history_size_; a++) {
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

int GenericCrowding::TournamentSelect() {
    
    int max_score = MIN_FITNESS;
    int winner = -1;
    for (int i = 0; i < tournament_size_; i++) {
        int j = rutil::pick_a_number(0,population_size_);
        if (population_[j].fitness > max_score) {
            max_score = population_[j].fitness;
            winner = j;
        }
    }
    return winner;
}

void GenericCrowding::Crossover(int parentA, int parentB) {

    int point = rutil::pick_a_number(0, genome_size_);
    for (int i = 0; i < point; i++) {
        population_[childA_].genome[i] = population_[parentA].genome[i];
        population_[childB_].genome[i] = population_[parentB].genome[i];
    }
    for (int i = point; i < genome_size_; i++) {
        population_[childA_].genome[i] = population_[parentB].genome[i];
        population_[childB_].genome[i] = population_[parentA].genome[i];
    }
}

void GenericCrowding::Mutate(int i) {
    
    for (int g = 0; g < genome_size_; g++) {
        if (rutil::pick_a_number(0.0, 1.0) < mutation_rate_){
            population_[i].genome[g] = RandomWeight();
        }
    }
}

int GenericCrowding::CrowdingSelect(int child) {
    
    int min_distance = MAX_DISTANCE;
    int loser = -1;
    for (int i = 0; i < tournament_size_; i++) {
        int j = rutil::pick_a_number(0, population_size_);
        int dist = HammingDistance(child, j);
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

void GenericCrowding::Replace(int loser, int winner) {
    if (population_[winner].fitness > best_fitness_) {
        best_fitness_ = population_[winner].fitness;
    }
    total_fitness_ -= population_[loser].fitness;
    total_fitness_ += population_[winner].fitness;
    static Individual tmp = population_[loser];
    population_[loser] = population_[winner];
    population_[winner] = tmp;
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

int GenericCrowding::HammingDistance(int i, int j) {
    
    int dist = 0;
    for (int a = 0; a < action_history_size_; a++) {
        if (population_[i].action_history[a] != population_[j].action_history[a]) {
            dist++;
        }
    }
    return dist;
}



