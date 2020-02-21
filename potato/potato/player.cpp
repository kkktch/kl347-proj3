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
#include "client.h"
#include "ringmaster.h"

using namespace std;

class player : public server, public client {
public:
    
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
        
        char port_ID[10];
        sprintf(port_ID, "%d", neighPort);
        Connect(neighAddr, port_ID, this->neigh);
        string IP;
        Connection(IP);
    }
    
    void Listening() {
        srand((unsigned int)time(NULL) + ID);
        potato currPotato;
        fd_set rfd;
        int nfd = 1;
        if (this->accept_fd > this->neigh) {
            nfd += this->accept_fd;
        } else {
            nfd += this->neigh;
        }
        while (1) {
            FD_ZERO(&rfd);
            FD_SET(this->accept_fd, &rfd);
            FD_SET(this->neigh, &rfd);
            FD_SET(this->master, &rfd);
            int res = select(nfd, &rfd, NULL, NULL, NULL);
            if (res < 0) {
                cerr << "Fail to select\n";
                exit(EXIT_FAILURE);
            } else if (res == 0) {
                cerr << "Time out\n";
                exit(EXIT_FAILURE);
            } else{
                if (FD_ISSET(this->accept_fd, &rfd)) {
                    if (recv(this->accept_fd, &currPotato, sizeof(currPotato), MSG_WAITALL) != sizeof(currPotato)) {
                        cerr << "Receive a broken potato\n";
                    }
                } else if (FD_ISSET(this->neigh, &rfd)) {
                    if (recv(this->neigh, &currPotato, sizeof(currPotato), MSG_WAITALL) != sizeof(currPotato)) {
                        cerr << "Receive a broken potato\n";
                    }
                } else if (FD_ISSET(this->master, &rfd)) {
                    if (recv(this->master, &currPotato, sizeof(currPotato), MSG_WAITALL) != sizeof(currPotato)) {
                        cerr << "Receive a broken potato\n";
                    }
                }
                
                if (currPotato.hop == 0) {
                    return;
                }
                
                currPotato.trace[currPotato.nums] = this->ID;
                ++currPotato.nums;
                --currPotato.hop;
                if (currPotato.hop == 0) {
                    if (send(this->master, &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
                        cerr << "Fail to send\n";
                    }
                    cout << "I'm it\n";
                    continue;
                }
                
                int randPlayer = rand() % 2;
                int targetPlayer;
                int sendingTarget;
                if (randPlayer == 0) {
                    targetPlayer = this->accept_fd;
                    sendingTarget = (ID == 0) ? totalNum - 1 : ID - 1;
                } else {
                    targetPlayer = this->neigh;
                    sendingTarget = (ID == totalNum-1) ? 0 : ID + 1;
                }
                cout << "Sending potato to " << sendingTarget << endl;
                if (send(targetPlayer, &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
                    cerr << "Fail to send\n";
                }
            }
        }
    }
    
    void init() {
        Neigh();
        Listening();
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



