//
//  rutil.cpp
//  ABQ_cpp
//
//  Created by Elliot Meyerson on 3/25/15.
//  Copyright (c) 2015 Elliot Meyerson. All rights reserved.
//

#include "rutil.h"
#include <iostream>

namespace rutil {
    
std::mt19937_64 & global_urng( ) {
    static std::mt19937_64 u{};
    return u;
}

void randomize( ) {
    static std::random_device rd{};
    global_urng().seed( rd() );
}

int pick_a_number( int from, int thru ) {
    static std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    //std::cout << d( global_urng(), parm_t{from, thru} );
    return d( global_urng(), parm_t{from, thru} );
}

double pick_a_number( double from, double upto ) {
    static std::uniform_real_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d( global_urng(), parm_t{from, upto} );
}

}