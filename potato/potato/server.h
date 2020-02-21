//
//  server.h
//  potato
//
//  Created by Kaidi Lyu on 2/14/20.
//  Copyright © 2020 Kaidi Lyu. All rights reserved.
//

#ifndef server_h
#define server_h
class server {
public:
    struct addrinfo master_info;
    struct addrinfo* info_list;
    int curr_status;
    int socket_fd;
    int accept_fd;
    
    void SetUp(const char* args) {
        memset(&master_info, 0, sizeof(master_info));
        master_info.ai_family = AF_UNSPEC;
        master_info.ai_socktype = SOCK_STREAM;
        master_info.ai_flags = AI_PASSIVE;
        if (strcmp(args, "") == 0) {
            this->curr_status = getaddrinfo(NULL, "", &master_info, &info_list);
        } else {
            this->curr_status = getaddrinfo(NULL, args, &master_info, &info_list);
        }
        
        if (this->curr_status != 0) {
            cerr << "Can't get correct address\n";
            exit(EXIT_FAILURE);
        }
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
        SetUp("");
        struct sockaddr_in* addr_in = (struct sockaddr_in*)this->info_list->ai_addr;
        addr_in->sin_port = 0;
        SetSocket();
        int currPort = GetPort();
        return currPort;
    }

    void Start(const char* args) {
        SetUp(args);
        SetSocket();
    }
    
    int Connection(string &IP){
        struct sockaddr_storage socket_addr;
        socklen_t len = sizeof(socket_addr);
        this->accept_fd = accept(this->socket_fd, (struct sockaddr*)&socket_addr, &len);
        if (this->accept_fd == -1) {
            cerr << "Fail to connect\n";
            exit(EXIT_FAILURE);
        }
        struct sockaddr_in* temp = (struct sockaddr_in*)&socket_addr;
        IP = inet_ntoa(temp->sin_addr);
        return this->accept_fd;
    }
    
    virtual ~server() {
        close(this->socket_fd);
    }
};

#endif /* server_h */
