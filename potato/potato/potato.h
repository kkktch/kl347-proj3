//
//  potato.h
//  potato
//
//  Created by Kaidi Lyu on 2/12/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//

#ifndef potato_h
#define potato_h

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class potato {
public:
    int hop;
    int port;
    int ID;
    int nums;
    vector<char> addr;
    vector<int> trace;
    
    potato() {
        addr.resize(512);
        trace.resize(512);
    }
};

class MetaInfo {
public:
    int neighPort;
    char neighAddr[100];
};

#endif /* potato_h */
