/*
 *  GeneralizedTartarus.cpp
 *  ABQ
 *
 *  Created by Elliot Meyerson on 3/24/15.
 *  Copyright 2015 Elliot Meyerson. All rights reserved.
 *
 */

#include "Tartarus.h"
#include <iostream>
#include "rutil.h"


Tartarus::Tartarus() {
    
    // Configuration parameters
    size_ = 6;
    num_bricks_ = 6;
    num_score_loc_ = 4;
    
    num_steps_ = 80;
    num_configs_ = 100;
    
    curr_step_ = 0;
    curr_config_ = 0;
    
    fitness_ = 0;
    sensors_.resize(8);
    
    InitBoard();
    InitScoreLocations();
    
    // Each config contains starting location of brick and agent
    configs_.resize(num_configs_);
    for (int i = 0; i < num_configs_; i++) {
        configs_[i].resize(num_bricks_+1);
        for (int j = 0; j < num_bricks_+1; j++) {
            configs_[i][j].resize(2);
        }
    }
    InitConfigs();
}

void Tartarus::InitBoard() {
    
    // Construct Board
    board_.resize(size_+2);
    for (int i = 0; i < size_+2; i++) {
        board_[i].resize(size_+2);
    }
    
    // Top and bottom walls
    for (int i = 0; i < size_+2; i++) {
        board_[i][size_+1] = WALL;
        board_[i][0] = WALL;
    }
    // Side walls
    for (int i = 1; i < size_+1; i++) {
        board_[0][i] = WALL;
        board_[size_+1][i] = WALL;
    }
}

void Tartarus::InitScoreLocations() {
    
    score_locations_.resize(num_score_loc_);
    for (int i = 0; i < num_score_loc_; i++){
        score_locations_[i].resize(2);
    }
    SetDefaultScoreLocations();
}

void Tartarus::SetDefaultScoreLocations() {
    // Default to one in each corner
    score_locations_[0][0] = 1;
    score_locations_[0][1] = 1;
    
    score_locations_[1][0] = 1;
    score_locations_[1][1] = 6;
    
    score_locations_[2][0] = 6;
    score_locations_[2][1] = 1;
    
    score_locations_[3][0] = 6;
    score_locations_[3][1] = 6;
}

void Tartarus::InitConfigs() {
    
    int configs_generated = 0;
    while (configs_generated < num_configs_) {
        ClearBoard();
        vector<int> cell;
        for (int i = 0; i < num_bricks_; i++) {
            cell = RandomInnerCell();
            board_[cell[0]][cell[1]] = BRICK;
            configs_[configs_generated][i][0] = cell[0];
            configs_[configs_generated][i][1] = cell[1];
        }
        if (ValidBoard()) {
            cell = RandomInnerCell();
            configs_[configs_generated][num_bricks_][0] = cell[0];
            configs_[configs_generated][num_bricks_][1] = cell[1];
            configs_generated++;
        }
    }
}

void Tartarus::ClearBoard() {
    
    for (int i = 1; i < size_+1; i++) {
        for (int j = 1; j < size_+1; j++) {
            board_[i][j] = EMPTY;
        }
    }
}

vector<int> Tartarus::RandomInnerCell() {
    
    while (true) {
        int x = rutil::pick_a_number(2, size_-1);
        int y = rutil::pick_a_number(2, size_-1);
        if (board_[x][y] == EMPTY) {
            vector<int> cell(2);
            cell[0] = x;
            cell[1] = y;
            return cell;
        }
    }
}

bool Tartarus::ValidBoard() {
    
    for (int x = 1; x < size_; x++) {
        for (int y = 1; y < size_; y++) {
            if (board_[x][y] == BRICK &&
                board_[x+1][y] == BRICK &&
                board_[x][y+1] == BRICK &&
                board_[x+1][y+1] == BRICK) {
                std::cout << "Invalid: \n";
                std::cout << toString();
                return false;
            }
        }
    }
    std::cout << "Valid: \n";
    std::cout << toString();
    return true;
}

void Tartarus::Reset() {
    
    fitness_ = 0;
    curr_config_ = 0;
    NextConfig();
    agent_orientation_ = NORTH;
}

void Tartarus::NextConfig() {
    
    ClearBoard();
    vector<int> cell;
    for (int i = 0; i < num_bricks_; i++) {
        cell = configs_[curr_config_][i];
        board_[cell[0]][cell[1]] = BRICK;
    }
    cell = configs_[curr_config_][num_bricks_];
    agent_x_ = cell[0];
    agent_y_ = cell[1];
    curr_step_ = 0;
}

vector<double>& Tartarus::Sense() { return sensors_; }

void Tartarus::Act(int action) {
    
    if (action == FORWARD) {
        Forward();
    } else if (action == LEFT) {
        agent_orientation_ = (agent_orientation_ + 3) % 4;
    } else if (action == RIGHT) {
        agent_orientation_ = (agent_orientation_ + 1) % 4;
    }
    
    curr_step_++;
    if (curr_step_ == num_steps_) {
        UpdateFitness();
        curr_config_++;
        if (!Done()) { NextConfig(); }
    }
}

void Tartarus::Forward() {
    
    // Get cell in front of agent
    int x1;
    int y1;
    if (agent_orientation_ == NORTH) {
        x1 = agent_x_;
        y1 = agent_y_+1;
    } else if (agent_orientation_ == EAST) {
        x1 = agent_x_+1;
        y1 = agent_y_;
    } else if (agent_orientation_ == SOUTH) {
        x1 = agent_x_;
        y1 = agent_y_-1;
    } else if (agent_orientation_ == WEST) {
        x1 = agent_x_-1;
        y1 = agent_y_;
    } else {
        std::cout << "HERE";
    }
    
    if (board_[x1][y1] == EMPTY) {
        agent_x_ = x1;
        agent_y_ = y1;
        UpdateSensors();
    } else if (board_[x1][y1] == BRICK) {
        
        // Get cell two ahead of agent
        int x2,y2;
        if (agent_orientation_ == NORTH) {
            x2 = agent_x_;
            y2 = agent_y_+2;
        } else if (agent_orientation_ == EAST) {
            x2 = agent_x_+2;
            y2 = agent_y_;
        } else if (agent_orientation_ == SOUTH) {
            x2 = agent_x_;
            y2 = agent_y_-2;
        } else if (agent_orientation_ == WEST) {
            x2 = agent_x_-2;
            y2 = agent_y_;
        }
        
        if (board_[x2][y2] == EMPTY) {
            board_[x2][y2] = BRICK;
            board_[x1][y1] = EMPTY;
            agent_x_ = x1;
            agent_y_ = y1;
            UpdateSensors();
        }
    }
    
}

void Tartarus::UpdateSensors() {
    
    int x = agent_x_;
    int y = agent_y_;
    UpdateSensor(0,x,y+1);
    UpdateSensor(1,x+1,y+1);
    UpdateSensor(2,x+1,y);
    UpdateSensor(3,x+1,y-1);
    UpdateSensor(4,x,y-1);
    UpdateSensor(5,x-1,y-1);
    UpdateSensor(6,x-1,y);
    UpdateSensor(7,x-1,y+1);
}

void Tartarus::UpdateSensor(int sensor_num, int x, int y) {
    
    int sensor_start = sensor_num;
    if (board_[x][y] == EMPTY) {
        sensors_[sensor_start] = 0.0;
    } else if (board_[x][y] == WALL) {
        sensors_[sensor_start] = 1.0;
    } else if (board_[x][y] == BRICK) {
        sensors_[sensor_start] = 1.0;
    }
}

void Tartarus::UpdateFitness() {
    
    int config_fitness = 0;
    // Check against top and bottom walls
    for (int i = 1; i < size_+1; i++) {
        if (board_[i][1] == BRICK) { config_fitness++; }
        if (board_[i][size_] == BRICK) { config_fitness++; }
    }
    // Check against side walls
    for (int i = 2; i < size_; i++) {
        if (board_[1][i] == BRICK) { config_fitness++; }
        if (board_[size_][i] == BRICK) { config_fitness++; }
    }
    // Check score locations
    for (int i = 0; i < num_score_loc_; i++) {
        //std::cout << toString();
        //std::cout << score_locations_[i][0];
        //std::cout << score_locations_[i][1];
        if (board_[score_locations_[i][0]][score_locations_[i][1]] == BRICK) {
            config_fitness++;
        }
    }
    
    //if (config_fitness > 0) {
    //std::cout << toString();
    //std::cout << "\n";
    //std::cout << config_fitness;
    //std::cout << "\n";
    //}
    fitness_ += config_fitness;
}

bool Tartarus::Done() { return curr_config_ == num_configs_; }

int Tartarus::fitness() { return fitness_; }

std::string Tartarus::toString() {
    
    std::string s;
    for (int y = size_+1; y > -1; y--) {
        s.append("\n");
        for (int x = 0; x < size_+2; x++) {
            if (board_[x][y] == WALL) {
                s.append("|");
            } else if (board_[x][y] == BRICK) {
                s.append("O");
            } else if (agent_x_ == x && agent_y_ == y) {
                if (agent_orientation_ == NORTH) {
                    s.append("^");
                } else if (agent_orientation_ == EAST) {
                    s.append(">");
                } else if (agent_orientation_ == SOUTH) {
                    s.append("<");
                } else if (agent_orientation_ == WEST) {
                    s.append("v");
                }
            } else {
                bool is_score_location = false;
                for (int i = 0; i < num_score_loc_; i++) {
                    if (score_locations_[i][0] == x && 
                        score_locations_[i][1] == y) {
                        s.append("X");
                        is_score_location = true;
                    }
                }
                if (!is_score_location) { s.append(" "); }
            }
            //s.append(to_string(y));
        }
        s.append("\n");
    }
    //for (int x = 0; x < size_+2; x++) { s.append(to_string(x)); }
    s.append("\n\n");
    return s;
}

// Cell states
const int Tartarus::EMPTY = 0;
const int Tartarus::WALL = 1;
const int Tartarus::BRICK = 2;

// Actions
const int Tartarus::FORWARD = 0;
const int Tartarus::LEFT = 1;
const int Tartarus::RIGHT = 2;

// Orientation
const int Tartarus::NORTH = 0;
const int Tartarus::EAST = 1;
const int Tartarus::SOUTH = 2;
const int Tartarus::WEST = 3;




















