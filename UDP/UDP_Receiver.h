//
// Created by ben on 2026/09/24.
//

#ifndef GPS_ONE_UDP_RECEIVER_H
#define GPS_ONE_UDP_RECEIVER_H

#pragma once // prevents the file from being included twice by accident
#include <string> // Pulls in string type

class UDP_Receiver {

    public:
        UDP_Receiver(int port);
        std::string ReceiveMessage(); // std::string -> string type from standard library

    private:
        int sockfd; // Holds our network socket (Integer ID), which our OS gives us
};


#endif //GPS_ONE_UDP_RECEIVER_H
