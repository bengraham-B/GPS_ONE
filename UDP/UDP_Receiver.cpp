#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "UDP_Receiver.h"

#define MAXLINE 1024

// Constructor
UDP_Receiver::UDP_Receiver(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        throw std::runtime_error("Error creating socket");
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        throw std::runtime_error("Bind Failed");
    }
}

std::string UDP_Receiver::ReceiveMessage()
{
    char buffer[MAXLINE];
    sockaddr_in cliaddr{};
    socklen_t len = sizeof(cliaddr);

    int n = recvfrom(sockfd, buffer, MAXLINE, 0, (sockaddr*)&cliaddr, &len);
    if ( n < 0)
    {
        throw std::runtime_error("Error receiving message");
    }

    return std::string(buffer, n);
}