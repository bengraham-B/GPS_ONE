#include <iostream>
#include "UDP/UDP_Receiver.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    std::cout << "GPS_ONE" <<std::endl;
    const int port = 4002;

    try {
        UDP_Receiver receiver(port);
        receiver.UDP_ReceiverService();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() <<std::endl;
    }

    return 0;
}