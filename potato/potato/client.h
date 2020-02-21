//
//  client.h
//  potato
//
//  Created by Kaidi Lyu on 2/19/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//

#ifndef client_h
#define client_h
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
#include <string.h>
#include "potato.h"

class client {
public:
    int ID;
    int totalNum;
    int master;
    int neigh;
    
    void Connect(const char* MasterName, const char* port, int& sockets, struct addrinfo master_info) {
        struct addrinfo* info_list;
        bzero(&master_info, sizeof(master_info));
        master_info.ai_family = AF_UNSPEC;
        master_info.ai_socktype = SOCK_STREAM;
        
        int status = getaddrinfo(MasterName, port, &master_info, &info_list);
        if (status != 0) {
            cerr << "Fail to get address info\n";
            cerr << "Master name: " << MasterName << ", port: " << port << endl;
            exit(EXIT_FAILURE);
        }
        
        sockets = socket(info_list->ai_family, info_list->ai_socktype, info_list->ai_protocol);
        if (sockets == -1) {
            cerr << "Fail to set socket\n";
            exit(EXIT_FAILURE);
        }
        
        status = connect(sockets, info_list->ai_addr, info_list->ai_addrlen);
        if (status == -1) {
            cerr << "Fail to connect to socket\n";
            exit(EXIT_FAILURE);
        }
        
        freeaddrinfo(info_list);
        
    }
};


#endif /* client_h */
