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
#include <sys/select.h>
#include <sys/time.h>
#include <functional>
#include <cstring>
#include <arpa/inet.h>
#include "potato.h"
#include "ringmaster.h"

using namespace std;

void ringmaster::BuildConnection() {
    for (size_t i = 0; i < this->sockets.size(); ++i) {
        this->sockets[i] = connection(this->IPAddrs[i]);
        send(this->sockets[i], &i, sizeof(i), 0);
        send(this->sockets[i], &this->player_num, sizeof(this->player_num), 0);
        recv(this->sockets[i], &this->ports[i], sizeof(this->ports[i]), MSG_WAITALL);
        cout << "Player " << i << "is ready to play\n";
    }
}

void ringmaster::CreatCycle() {
    for (int i = 0; i < this->player_num; ++i) {
        int neighID = (i == this->player_num - 1) ? 0 : i + 1;
        int neighPort = this->ports[neighID];
        char neighAddr[100];
        strcpy(neighAddr, this->IPAddrs[neighID].c_str());
        send(this->sockets[i], &neighPort, sizeof(neighPort), 0);
        send(this->sockets[i], &neighAddr, sizeof(neighAddr), 0);
    }
}

void ringmaster::SendSockets(potato currPotato) {
    for (int i = 0; i < this->player_num; ++i) {
        if (send(this->sockets[i], &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
            cerr << "Broken potato\n";
        }
    }
}

void ringmaster::GamePlay() {
    potato currPotato;
    currPotato.hop = this->hop_num;
    if (currPotato.hop != 0) {
        int randPlayer = rand() % this->player_num;
        cout << "Ready to start the game, sending potato to player" << randPlayer << endl;
        if (send(this->sockets[randPlayer], &currPotato, sizeof(currPotato), 0) != sizeof(currPotato)) {
            cerr << "Broken potato\n";
        }
        fd_set rfd;
        FD_ZERO(&rfd);
        int maxFD = 0;
        for (int i = 0; i < this->player_num; ++i) {
            FD_SET(this->sockets[i], &rfd);
            if (this->sockets[i] > maxFD) {
                maxFD = this->sockets[i];
            }
        }
        int res = select(this->accept_fd + 1, &rfd, NULL, NULL, NULL);
        if (res < 0) {
            cerr << "Fail to select\n";
            exit(EXIT_FAILURE);
        } else if (res == 0) {
            cerr << "Time out\n";
            exit(EXIT_FAILURE);
        } else{
            for (int i = 0; i < this->player_num; ++i) {
                if (FD_ISSET(this->sockets[i], &rfd)) {
                    if(recv(this->sockets[i], &currPotato, sizeof(currPotato), MSG_WAITALL) < 0) {
                        cerr << "Fail to receive a potato\n";
                        exit(EXIT_FAILURE);
                    }
                    SendSockets(currPotato);
                    break;
                }
            }
            return;
        }
    } else {
        SendSockets(currPotato);
        return;
    }
}

void  ringmaster::ringmaster(char** args){
    this->player_num = atoi(args[2]);
    this->hop_num = atoi(args[3]);
    this->ports.resize(this->player_num);
    this->sockets.resize(this->player_num);
    this->IPAddrs.resize(this->player_num);
    this->Start(args[1]);
}




int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: ./ringmaster <port_num> <num_player> <num_hops>\n";
        exit(EXIT_FAILURE);
    }else if (!(atoi(argv[3]) <= 512 && atoi(argv[3]) >= 0)){
        cerr << "num_hops must be greater than or equal to 0 and less than or equal to 512\n";
        exit(EXIT_FAILURE);
    }else if (atoi(argv[2]) <= 1) {
        cerr << "num_players must be greater than 1\n";
        exit(EXIT_FAILURE);
    }else{
        ringmaster* currMaster = new ringmaster(argv);
        cout << "Potato Ringmaster\n";
        cout << "Players = " << this->player_num << endl;
        cout << "Hops = " << this->hop_num << endl;
        currMaster->BuildConnection();
        currMaster->CreatCycle();
        currMaster->GamePlay();
        currMaster->Close();
        delete currMaster;
        return EXIT_SUCCESS;
    }
}
