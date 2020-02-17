//
//  ringmaster.cpp
//  potato
//
//  Created by Kaidi Lyu on 2/12/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
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
    vector<int> ports;
    vector<int> sockets;
    vector<string> IPAddrs;
    
    void SetUp(const char* args) {
        memset(&master_info, 0, sizeof(master_info));
        master_info.ai_family = AF_UNSPEC;
        master_info.ai_socktype = SOCK_STREAM;
        master_info.ai_flags = AI_PASSIVE;
        if (args != NULL) {
            this->curr_status = getaddrinfo(NULL, args, &master_info, &info_list);
        } else {
            this->curr_status = getaddrinfo(NULL, NULL, &master_info, &info_list);
        }
        if (this->curr_status != 0) {
            cerr << "Can't get correct address\n";
            exit(EXIT_FAILURE);
        }
    }
    
    void SetPort() {
        struct sockaddr_in* add_in = (struct sockaddr_in*)this->info_list->ai_addr;
        add_in->sin_port = 0;
    }
    
    int GetPort() {
        struct sockaddr_in sockin;
        socklen_t length = sizeof(sockin);
        int portID = 0;
        if (getsockname(this->socket_fd, (struct sockaddr*)&sockin, &length) == -1) {
            cerr << "getsockname() failed\n";
            exit(EXIT_FAILURE);
        }
        portID = ntohs(sockin.sin_port);
        return portID;
    }
    
    void SetSocket() {
        this->socket_fd = socket(this->info_list->ai_family, this->info_list->ai_socktype, this->info_list->ai_protocol);
        if (this->socket_fd == -1) {
            cerr << "Fail to set socket\n";
            exit(EXIT_FAILURE);
        }
        
        int opt = 1;
        this->curr_status = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        this->curr_status = ::bind(this->socket_fd, this->info_list->ai_addr, this->info_list->ai_addrlen);
        if (this->curr_status == -1) {
            cerr << "Fail to bind socket\n";
            exit(EXIT_FAILURE);
        }
        
        this->curr_status = listen(this->socket_fd, 100);
        if (this->curr_status == -1) {
            cerr << "Fail to listen on socket\n";
            exit(EXIT_FAILURE);
        }
        
        freeaddrinfo(this->info_list);
    }
    
    int Start() {
        SetUp(NULL);
        SetPort();
        SetSocket();
        int currPort = GetPort();
        return currPort;
    }
    
    void Start(const char* args) {
        SetUp(args);
        SetSocket();
    }
    
    void BuildConnection() {
        for (size_t i = 0; i < this->sockets.size(); ++i) {
            struct sockaddr_storage sockaddr;
            socklen_t len = sizeof(sockaddr);
            this->sockets[i] = accept(this->socket_fd, (struct sockaddr*)&sockaddr, &len);
            if (this->sockets[i] == -1) {
                return;
            }
            struct sockaddr_in* IPtemp = (struct sockaddr_in*)&sockaddr;
            this->IPAddrs[i] = inet_ntoa(IPtemp->sin_addr);
            send(this->sockets[i], &i, sizeof(i), 0);
            send(this->sockets[i], &this->player_num, sizeof(this->player_num), 0);
            recv(this->sockets[i], &this->ports[i], sizeof(this->ports[i]), MSG_WAITALL);
            cout << "Player " << i << "is ready to play\n";
        }
    }
    
    void CreatCycle() {
        for (int i = 0; i < this->player_num; ++i) {
            int neighID = (i == this->player_num - 1) ? 0 : i + 1;
            int neighPort = this->ports[neighID];
            char neighAddr[100];
            strcpy(neighAddr, this->IPAddrs[neighID].c_str());
            send(this->sockets[i], &neighPort, sizeof(neighPort), 0);
            send(this->sockets[i], &neighAddr, sizeof(neighAddr), 0);
        }
    }
    
    void GamePlay() {
        potato currPotato;
        currPotato.hop = this->hop_num;
        if (currPotato.hop == 0) {
            for (int i = 0; i < this->player_num; ++i) {
                if (send(this->sockets[i], &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
                    cerr << "Broken potato\n";
                }
            }
            return;
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    
    ~ringmaster(){
        close(this->socket_fd);
        for (size_t i = 0; i < sockets.size(); ++i) {
            close(this->sockets[i]);
        }
    }
    
    ringmaster(const char** args){
        this->player_num = atoi(args[2]);
        this->hop_num = atoi(args[3]);
        this->ports.resize(this->player_num);
        this->sockets.resize(this->player_num);
        this->IPAddrs.resize(this->player_num);
        this->Start(args[1]);
    }
};
