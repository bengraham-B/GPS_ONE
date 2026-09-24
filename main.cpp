#include <iostream>
#include "UDP/UDP_Receiver.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    int port = 8080;
    try {
       UDP_Receiver receiver(port); // setting port on which to receive UDP
        std::string message = receiver.ReceiveMessage();
        std::cout<< "Listening on " << port <<std::endl;
        std::cout << "Received: " << message << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() <<std::endl;
    }

    return 0;
}