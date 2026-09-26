#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "UDP_Receiver.h"

using namespace std;

#define MAXLINE 1024

//Constructor
UDP_Sends::UDP_Sends(const int port, const string serverURL, const string message)
{
    this->port = port;
    this->serverURL = serverURL;
    this->message = message;
    this->sockfd = -1;
}

void UDP_Sends::SendMessage() const
{
    /*
     * This asks the OS for a new Socket Setup
     * AF_INET -> use IPv4 Addressing
     * SOCK_DGRAM -> datagram (UDP) socket
     * 0 -> Lets the OS pick the default protocol for this socket type, UDP.
     */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Returns a negative value if the socket creation failed
    if (sock < 0)
    {
        perror("Socket creation failed");
        return; // Exit out using a return to make sure that 1 failed socket does not cause the application to fail.
    }

    /*
     * sockaddr -> describes an IPv4 address + PORT in the format the OS networking calls expect.
     */
   struct sockaddr_in servaddr{};

    servaddr.sin_family = AF_INET; // This is an IPv4 address
    /*
     * htons = "host to network short" — converts our port number from
     * this machine's native byte order into network byte order, which
     * is the byte order all IP networking expects. Forgetting htons()
     * is a classic bug that silently sends to the wrong port.
    */
    servaddr.sin_port = htons(port);

    /*
     * inet_pton -> Converts human readable IPv4 address into the binary format need by the socket API
     * This is written directly to  servaddr.sin_addr.
     */
    if (inet_pton(AF_INET, serverURL.c_str(), &servaddr.sin_addr) <= 0)
    {
        cerr << "Invalid Address: " << serverURL <<endl;
        close(sock); // Closing the socket we opened before it leaks
        return;
    }

    /* This is the actual send
     * sock => Which socket to send from
     * message.c_str() => pointer to the raw bytes of out message
     * message.size() => how many bytes to send (Real message to send size).
     * (sockaddr*)&servaddr, sizeof(servaddr) => Where to send it: destination + its size, cast to the generic sockaddr type the OS API expects
     *
     * sendto() => returns the number of bytes actually sent or -1 on error, it does not throw, so we must check the return values ourseld
     */

    ssize_t sent = sendto(sock, message.c_str(), message.size(), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    if (sent < 0)
    {
        // sendto failed - e.g network unreachable, permission issue.
        perror("sendto failed");
    }
    else
    {
        cout << "Sent: " << sent << " | " << serverURL << ":" << port << endl;
    }
    // Confirms socket opened with socket() must eventually be closed
    // close(), or the file descriptor says reserved by the OS forever
    // (a "leak").

    // Since we open a fresh socket every call , we must also
    // close it every call. This is the line the original code was missing entirely.
    close(sock);
















}