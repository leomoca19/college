#include <iostream>
#include <map>
#include "socket.hpp"
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8080

using std::string, std::cin, std::cout, std::cerr, std::endl;

int main() {
    std::map<string, string> users = {
        { "1", "pswd1" },
        { "2", "pswd2" },
        { "3", "pswd3" }
    };

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
    
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    // Bind socket to address
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    // Start listening
    listen(serverSocket, 3);
    cout << "Server listening on port " << PORT << "..." << endl;
    
    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    cout << "Client connected" << endl;
    
    string str;
    
    auto receive = [&clientSocket]() -> string {
        constexpr int sz{1024};
        char buff[sz]{};
        int len = recv(clientSocket, buff, sz, 0);
        return (0 < len ? string(buff, len) : "");
    };
    auto send_msg = [&clientSocket](string msg) {
        return send(clientSocket, msg.c_str(), msg.size(), 0);
    };

    // login
    bool success{};
    while (!success) {
        send_msg("Enter id,password: ");
        str = receive(); //id,password

        const int delim = str.find(',');
        string id = str.substr(0, delim);
        string pswd = str.substr(delim + 1, str.size());

        success = users.contains(id) && users[id] == pswd;
        send_msg(!success ? "Fail\n" : "Success");
    }

    cout << "Closing server" << endl;

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}