#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void error_exit(const char* message) {
    std::cerr << message << " Error Code: " << WSAGetLastError() << std::endl;
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) error_exit("WSAStartup failed");

    // Create UDP socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        error_exit("Socket creation failed");

    // Server address configuration
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Open file for reading
    std::ifstream inFile("C:/Users/Zara/source/repos/FileClient/fileToTransfer.txt", std::ios::binary);
    if (!inFile) error_exit("Error opening file");

    while (!inFile.eof()) {
        inFile.read(buffer, BUFFER_SIZE);
        int bytesRead = inFile.gcount();

        if (sendto(clientSocket, buffer, bytesRead, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
            error_exit("sendto failed");
    }

    // Send end signal
    strcpy_s(buffer, "END");
    sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    std::cout << "File transfer complete.\n";

    inFile.close();
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
