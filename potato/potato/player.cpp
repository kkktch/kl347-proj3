//
//  player.cpp
//  potato
//
//  Created by Kaidi Lyu on 2/13/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//
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
#include "ringmaster.h"

using namespace std;

class player : public server {
public:
    int ID;
    int totalNum;
    int master;
    int neigh;
    
    void Connect(const char* MasterName, const char* port, int& sockets) {
        struct addrinfo master_info;
        struct addrinfo* info_list;
        memset(&master_info, 0, sizeof(master_info));
        master_info.ai_family = AF_UNSPEC;
        master_info.ai_socktype = SOCK_STREAM;
        
        this->curr_status = getaddrinfo(MasterName, port, &master_info, &info_list);
        if (this->curr_status != 0) {
            cerr << "Fail to get address info\n";
            cerr << "Master name: " << MasterName << ", port: " << port << endl;
            exit(EXIT_FAILURE);
        }
        
        sockets = socket(info_list->ai_family, info_list->ai_socktype, info_list->ai_protocol);
        if (sockets == -1) {
            cerr << "Fail to set socket\n";
            exit(EXIT_FAILURE);
        }
        
        this->curr_status = connect(sockets, info_list->ai_addr, info_list->ai_addrlen);
        if (this->curr_status == -1) {
            cerr << "Fail to connect to socket\n";
            exit(EXIT_FAILURE);
        }
        
        freeaddrinfo(info_list);
        
    }
    
    void Master(const char* MasterName, const char* port) {
        Connect(MasterName, port, this->master);
        recv(this->master, &ID, sizeof(ID), 0);
        recv(this->master, &totalNum, sizeof(totalNum), 0);
        
        int ListenPort = Start();
        send(this->master, &ListenPort, sizeof(ListenPort), 0);
        cout << "Connected as player " << this->ID << " out of " << this->totalNum << " total players\n";
    }
    
    void Neigh() {
        int neighPort;
        char neighAddr[100];
        recv(this->master, &neighPort, sizeof(neighPort), MSG_WAITALL);
        recv(this->master, &neighAddr, sizeof(neighAddr), MSG_WAITALL);
        
        char port_ID[9];
        sprintf(port_ID, "%d", neighPort);
        Connect(neighAddr, port_ID, this->neigh);
        string IP;
        Connection(IP);
    }
    
    void Listening() {
        srand((unsigned int)time(NULL) + ID);
        potato currPotato;
        fd_set rfd;
        int sockets[] = {this->accept_fd, this->neigh, this->master};
        int nfd = 1;
        if (this->accept_fd > this->neigh) {
            nfd += this->accept_fd;
        } else {
            nfd += this->neigh;
        }
        while (1) {
            FD_ZERO(&rfd);
            FD_SET(sockets[0], &rfd);
            FD_SET(sockets[1], &rfd);
            FD_SET(sockets[2], &rfd);
            int res = select(nfd, &rfd, NULL, NULL, NULL);
            if (res < 0) {
                cerr << "Fail to select\n";
                exit(EXIT_FAILURE);
            } else if (res == 0) {
                cerr << "Time out\n";
                exit(EXIT_FAILURE);
            } else{
                for (int i = 0; i < 3; ++i) {
                    if (FD_ISSET(sockets[i], &rfd)) {
                        if (recv(sockets[i], &currPotato, sizeof(currPotato), MSG_WAITALL) != sizeof(currPotato)) {
                            cerr << "Receive a broken potato\n";
                        }
                        break;
                    }
                }
                
                if (currPotato.hop == 0) {
                    return;
                }
                
                --currPotato.hop;
                currPotato.trace[currPotato.nums] = this->ID;
                ++currPotato.nums;
                if (currPotato.hop == 0) {
                    if (send(this->master, &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
                        cerr << "Fail to send\n";
                    }
                    cout << "I'm it\n";
                    continue;
                }
                
                int randPlayer = rand() % 2;
                int targetPlayer;
                if (randPlayer == 0) {
                    targetPlayer = (this->ID - 1 + this->totalNum) % this->totalNum;
                } else {
                    targetPlayer = (this->ID + 1) % this->totalNum;
                }
                cout << "Sending potato to " << targetPlayer << endl;
                if (send(sockets[randPlayer], &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
                    cerr << "Fail to send\n";
                }
            }
        }
    }
    
    void init() {
        Neigh();
        Listening();
        sleep(1);
    }
    
    player(char* name, char* port) {
        Master(name, port);
    }
    
    virtual ~player() {
        close(this->accept_fd);
        close(this->master);
        close(this->neigh);
    }
};

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: ./player <machine_name> <port_num>\n";
        exit(EXIT_FAILURE);
    } else {
        player* currPlayer = new player(argv[1], argv[2]);
        currPlayer->init();
        delete currPlayer;
        return EXIT_SUCCESS;
    }
}



