#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8080

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);
    char buffer[1024] = { 0 };

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket to address
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Start listening
    listen(serverSocket, 3);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);

    // Receive data from client
    recv(clientSocket, buffer, 1024, 0);
    std::string clientChoice(buffer);
    std::string response;

    // Process client choice
    if (clientChoice == "Coffee") {
        response = "Your coffee is being prepared!";
    }
    else if (clientChoice == "Tea") {
        response = "Your tea is being prepared!";
    }
    else {
        response = "Invalid choice";
    }

    // Send response to client
    send(clientSocket, response.c_str(), response.size(), 0);

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
