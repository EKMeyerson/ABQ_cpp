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
    static const long EMPTY;
    static const long WALL;
    static const long BRICK;
    
    // Actions
    static const long FORWARD;
    static const long LEFT;
    static const long RIGHT;
    
    // Orientation
    static const long NORTH;
    static const long EAST;
    static const long SOUTH;
    static const long WEST;
    
    // Configuration parameters
    long size_, num_bricks_, num_score_loc_;
    long num_steps_, num_configs_;
    
    // State information
    long curr_step_, curr_config_;
    long agent_x_, agent_y_, agent_orientation_;
    long fitness_;
    vector< vector<long> > score_locations_;
    vector< vector<long> > board_;
    vector< vector< vector<long> > > configs_;
    vector<double> sensors_;
    
    // Initialization methods
    void InitBoard();
    void InitScoreLocations();
    void SetDefaultScoreLocations();
    void InitConfigs();
    vector<long> RandomInnerCell();
    vector<long> RandomWallCell();
    bool ValidBoard();
    
    // Evaluation methods
    void NextConfig();
    void ClearBoard();
    void UpdateSensors();
    void UpdateSensor(long,long,long);
    void Forward();
    void Left();
    void Right();
    void UpdateFitness();
    
public:
    Tartarus();
    void Reset();
    vector<double>& Sense();
    void Act(long);
    bool Done();
    long fitness();
    std::string toString();
};