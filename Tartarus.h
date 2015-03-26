/*
 *  Tartarus.h
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include <vector>
#include <stdlib.h>
#include <string>
#include <random>

using std::vector;


class Tartarus {
    
    // Cell states
    static const int EMPTY;
    static const int WALL;
    static const int BRICK;
    
    // Actions
    static const int FORWARD;
    static const int LEFT;
    static const int RIGHT;
    
    // Orientation
    static const int NORTH;
    static const int EAST;
    static const int SOUTH;
    static const int WEST;
    
    // Configuration parameters
    int size_, num_bricks_, num_score_loc_;
    int num_steps_, num_configs_;
    
    // State information
    int curr_step_, curr_config_;
    int agent_x_, agent_y_, agent_orientation_;
    int fitness_;
    vector< vector<int> > score_locations_;
    vector< vector<int> > board_;
    vector< vector< vector<int> > > configs_;
    vector<double> sensors_;
    //std::seed_seq seed2_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dis_;
    
    // Initialization methods
    void InitBoard();
    void InitScoreLocations();
    void SetDefaultScoreLocations();
    void InitConfigs();
    vector<int> RandomInnerCell();
    vector<int> RandomWallCell();
    int RandInt(int,int);
    bool ValidBoard();
    
    // Evaluation methods
    void NextConfig();
    void ClearBoard();
    void UpdateSensors();
    void UpdateSensor(int,int,int);
    void Forward();
    void Left();
    void Right();
    void UpdateFitness();
    
public:
    Tartarus();
    void Reset();
    vector<double>& Sense();
    void Act(int);
    bool Done();
    int fitness();
    std::string toString();
};