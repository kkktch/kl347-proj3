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
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class potato {
public:
    int hop;
    int port;
    int ID;
    int nums;
    vector<char> addr;
    int trace[512];
    
    potato() {
        hop = 0;
        nums = 0;
        memset(trace, 0, sizeof(trace));
    }
    
    void Trace() {
        cout << "Trace of potato:\n";
        for (int i = 0; i < nums; ++i) {
            cout << trace[i];
            string END = (i == nums-1) ? "\n" : ", ";
            cout << END;
        }
    }
    
};


#endif /* potato_h */
