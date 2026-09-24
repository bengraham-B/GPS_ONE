#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <cstring>

#include "UDP_Receiver.h"

#define MAXLINE 1024

// Constructor
UDP_Receiver::UDP_Receiver(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    this->port = port; // save constructor argument into the field;

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

    // return std::string(buffer, n);

    std::string message(buffer, n);

    //TRrim trailing \r and \n characters (NMEA sentences end with \r\n)
    while (!message.empty() && (message.back() == '\r' || message.back() == '\n')) {
        message.pop_back();
    }

    return message;

}

void UDP_Receiver::UDP_ReceiverService()
{
    printf("UDP Receiver Running on PORT: %d\n", port);
    while (true) {
        std:: string message = ReceiveMessage();
        std::cout << message << std::endl;
    }
}

std::string UDP_Receiver::GetIPAddress()
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct ifreq ifr{};
    strcpy(ifr.ifr_name, "wlo1");
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    char ip[INET_ADDRSTRLEN];
    strcpy(ip, inet_ntoa(((sockaddr_in *) &ifr.ifr_addr)->sin_addr));

    std::cout << ip << std::endl;

    return ip;

}