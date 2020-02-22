//
//  ringmaster.h
//  potato
//
//  Created by Kaidi Lyu on 2/13/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//

#ifndef ringmaster_h
#define ringmaster_h
#include <stdio.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <functional>
#include <cstring>
#include <arpa/inet.h>
#include "potato.h"
#include "server.h"

using namespace std;

class ringmaster : public server {
public:
    int player_num;
    int hop_num;
    vector<int> ports;
    vector<int> sockets;
    vector<string> IPAddrs;
    
    void CreatCycle();
    void SendSockets(potato currPotato);
    void GamePlay();
    void Close(){
        for (size_t i = 0; i < sockets.size(); ++i) {
            close(this->sockets[i]);
        }
    }
    ringmaster(char** args);
};
    

#endif /* ringmaster_h */
