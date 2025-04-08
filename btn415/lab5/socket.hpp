#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#include <string>
#include <winsock2.h>
#include <iostream>
#include <stdexcept>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

using std::string;
class TcpSock{
	SOCKET clientSocket;
	struct sockaddr_in addr;
	int addrLen;
	bool isServer;
	WSADATA wsaData;
public:
	SOCKET sock;
	TcpSock(const bool& isServer_=false): isServer(isServer_){
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		sock = socket(AF_INET, SOCK_STREAM, 0);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(PORT);
		addrLen = sizeof(addr);
	}

	SOCKET& startServer() {
		if (isServer) {
			bind(sock, (struct sockaddr*)&addr, sizeof(addr));
			listen(sock, 5);
			clientSocket = accept(sock, (struct sockaddr*)&addr, &addrLen);
			return clientSocket;
		}
		else {
			std::cerr << "Socket is not server" << std::endl;
			throw std::runtime_error("Connection failed");
		}
	}

	inline int send(const string& msg) {
		return ::send(sock, msg.c_str(), msg.size(), 0);
	}

	inline string recv() {
		constexpr int sz{1024};
		char buff[sz]{};
		int len = ::recv(sock, buff, sz, 0);
		if (0 > len) {
			len = 0;
			std::cerr << "Failed recv()\n";
		}
		return string(buff, len);
	}

	~TcpSock(){
		closesocket(sock);
		WSACleanup();
	}
};
