#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    SOCKET clientSocket;
    struct sockaddr_in servAddr;
    char buffer[1024] = { 0 };

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Connect to server
    connect(clientSocket, (struct sockaddr*)&servAddr, sizeof(servAddr));

    // Display menu
    std::cout << "Choose an option:\n1. Coffee\n2. Tea\nEnter your choice: ";
    std::string choice;
    std::cin >> choice;

    // Send choice to server
    send(clientSocket, choice.c_str(), choice.length(), 0);

    // Receive and display response
    recv(clientSocket, buffer, 1024, 0);
    std::cout << "Server Response: " << buffer << std::endl;

    // Close socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
