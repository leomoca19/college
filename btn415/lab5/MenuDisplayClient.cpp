#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#include <iostream>

#include "inventory.hpp"

// #pragma is not supported by my compiler g++
// I am dissabling the comment if the compiler being used is from
// Microsoft/Embarcadero source:
// https://stackoverflow.com/questions/71831583/ignoring-pragma-comment-ws2-32-lib-wunknown-pragmas
#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library
#endif

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

using std::string, std::cin, std::cout, std::cerr, std::endl;

int main() {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock

  SOCKET clientSocket;
  struct sockaddr_in servAddr;

  // Create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(PORT);
  servAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

  // Connect to server
  if (0 ==
      connect(clientSocket, (struct sockaddr*)&servAddr, sizeof(servAddr))) {
    auto receive = [&clientSocket]() -> string {
      constexpr int sz{1024};
      char buff[sz]{};
      int len = recv(clientSocket, buff, sz, 0);
      return (0 < len ? string(buff, len) : "");
    };
    auto send_msg = [&clientSocket](string msg) {
      return send(clientSocket, msg.c_str(), msg.size(), 0);
    };

    string str;

    // login
    while ("Success" != str) {
      cout << receive();

      cin >> str;
      send_msg(str);

      str = receive();
      cout << str << endl;
    }
    const char* id = receive().c_str();

    // menu interaction
    constexpr const char* menuOptions =
        "Menu options\n"
        "0 Exit\n"
        "1 View local inventory\n"
        "2 View server's inventory\n"
        "3 Buy from server\n"
        ": ";
    do {
      string localFname = string("client") + id + ".txt";
      Inventory local(localFname.c_str());
      cout << menuOptions;  // menu options
      cin >> str;

      if ("0" == str) {
        send_msg(str);
      } else if ("1" == str) {
        local.print();
      } else if ("2" == str) {
        cout << "Under construcion" << endl;
      } else if ("3" == str) {
        cout << "Under construcion" << endl;
      } else {
        cout << "Invalid input" << endl;
      }
    } while ("0" != str);

    cout << "You exit the program" << endl;
  } else {
    cerr << "connect() failed, error: " << WSAGetLastError() << endl;
  }

  // Close socket
  closesocket(clientSocket);
  WSACleanup();

  return 0;
}
