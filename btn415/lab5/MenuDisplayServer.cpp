#include <winsock2.h>

#include <iostream>
#include <map>

#include "inventory.hpp"

// #pragma is not supported by my compiler g++
// I am dissabling the comment if the compiler being used is from
// Microsoft/Embarcadero source:
// https://stackoverflow.com/questions/71831583/ignoring-pragma-comment-ws2-32-lib-wunknown-pragmas
#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library
#endif

#define PORT 8080

using std::string, std::cin, std::cout, std::cerr, std::endl;

int main() {
  std::map<string, string> users = {
      {"1", "pswd1"}, {"2", "pswd2"}, {"3", "pswd3"}};

  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock

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
  string id;
  while (!success) {
    send_msg("Enter id,password: ");
    str = receive();  // id,password

    const int delim = str.find(',');
    id = str.substr(0, delim);
    string pswd = str.substr(delim + 1, str.size());

    success = users.contains(id) && users[id] == pswd;
    send_msg(!success ? "Fail" : "Success");
  }
  send_msg(id);

  // menu interaction
  Inventory serverInv("server.txt");
  while ("0" != (str = receive())) {
    if ("2" == str) {
      cout << "Under construcion" << endl;
    } else if ("3" == str) {
      cout << "Under construcion" << endl;
    }
  }
  cout << "User exit the program" << endl;

  cout << "Closing server" << endl;

  serverInv.save();
  cout << "Inventory saved" << endl;

  // Close sockets
  closesocket(clientSocket);
  closesocket(serverSocket);
  WSACleanup();

  return 0;
}
