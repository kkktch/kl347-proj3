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

using namespace std;

class ringmaster {
public:
    struct addrinfo master_info;
    struct addrinfo* info_list;
    int player_num;
    int hop_num;
    int curr_status;
    int socket_fd;
    int accept_fd;
    vector<int> ports;
    vector<int> sockets;
    vector<string> IPAddrs;
    
    void SetUp(const char* args);
    void SetPort();
    int GetPort();
    void SetSocket();
    int Start();
    void Start(const char* args);
    void BuildConnection();
    void CreatCycle();
    void SendSockets(potato* currPotato);
    void GamePlay();
    virtual ~ringmaster(){
        close(this->socket_fd);
        for (size_t i = 0; i < sockets.size(); ++i) {
            close(this->sockets[i]);
        }
    }
    void  init(char** args);
};
    

#endif /* ringmaster_h */
