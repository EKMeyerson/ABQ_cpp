//
//  rutil.h
//  ABQ_cpp
//
//  Created by Elliot Meyerson on 3/25/15.
//  Copyright (c) 2015 Elliot Meyerson. All rights reserved.
//

#include <stdio.h>
#include <random>

namespace rutil {
    
    std::mt19937_64 & global_urng( );
    
    void randomize( );
    
    long pick_a_number( long from, long thru );
    
    double pick_a_number( double from, double upto );

}
