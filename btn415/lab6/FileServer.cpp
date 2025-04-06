#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void error_exit(const char* message) {
    std::cerr << message << " Error Code: " << WSAGetLastError() << std::endl;
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) error_exit("WSAStartup failed");

    // Create UDP socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        error_exit("Socket creation failed");

    // Server address configuration
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        error_exit("Bind failed");

    std::cout << "Server listening on port " << PORT << "...\n";

    std::ofstream outFile("received_file.txt", std::ios::binary);
    if (!outFile) error_exit("Error opening file");

    while (true) {
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytesReceived == SOCKET_ERROR)
            error_exit("recvfrom failed");

        if (strncmp(buffer, "END", 3) == 0) {
            std::cout << "File transfer complete.\n";
            break;
        }

        outFile.write(buffer, bytesReceived);
    }

    outFile.close();
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
