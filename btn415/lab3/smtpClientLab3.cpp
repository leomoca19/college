#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

void sendSMTPCommand(SSL* ssl, const std::string& command, const std::string& expectedResponse) {
    SSL_write(ssl, command.c_str(), command.size());
    char response[1024] = { 0 };
    SSL_read(ssl, response, sizeof(response) - 1);
    std::cout << "Server Response: " << response;
    if (expectedResponse != "" && std::string(response).find(expectedResponse) == std::string::npos) {
        throw std::runtime_error("Unexpected SMTP response.");
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed.\n";
        return 1;
    }

    // SMTP server details
    const char* smtpServer = "smtp.gmail.com";
    const int smtpPort = 465; // SMTP over SSL

    // Sender credentials
    const std::string emailSender = "your_email@gmail.com";
    const std::string emailPassword = "your_email_password";
    const std::string emailReceiver = "receiver_email@gmail.com";

    const std::string subject = "Test Email from Winsock C++";
    const std::string body = "This is a test email sent using Winsock and OpenSSL.";

    try {
        // Create socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed.");
        }

        // Resolve SMTP server address
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(smtpPort);
        struct hostent* host = gethostbyname(smtpServer);
        memcpy(&serverAddr.sin_addr, host->h_addr_list[0], host->h_length);

        // Connect to the SMTP server
        if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            closesocket(sock);
            throw std::runtime_error("Connection to SMTP server failed.");
        }

        // Initialize OpenSSL
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        SSL_CTX* ctx = SSL_CTX_new(SSLv23_client_method());
        if (!ctx) {
            closesocket(sock);
            throw std::runtime_error("Failed to create SSL context.");
        }

        // Create SSL structure and connect it to the socket
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        if (SSL_connect(ssl) <= 0) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            closesocket(sock);
            throw std::runtime_error("SSL connection failed.");
        }

        // SMTP Communication
        char buffer[1024];
        SSL_read(ssl, buffer, sizeof(buffer) - 1); // Read initial response

        sendSMTPCommand(ssl, "EHLO localhost\r\n", "250");
        sendSMTPCommand(ssl, "AUTH LOGIN\r\n", "334");

        // Encode credentials in Base64
        std::string base64Email = base64_encode(reinterpret_cast<const unsigned char*>(emailSender.c_str()), emailSender.size());
        std::string base64Password = base64_encode(reinterpret_cast<const unsigned char*>(emailPassword.c_str()), emailPassword.size());

        sendSMTPCommand(ssl, base64Email + "\r\n", "334");
        sendSMTPCommand(ssl, base64Password + "\r\n", "235");

        // Send email
        sendSMTPCommand(ssl, "MAIL FROM:<" + emailSender + ">\r\n", "250");
        sendSMTPCommand(ssl, "RCPT TO:<" + emailReceiver + ">\r\n", "250");
        sendSMTPCommand(ssl, "DATA\r\n", "354");

        std::string emailContent = "Subject: " + subject + "\r\n\r\n" + body + "\r\n.\r\n";
        sendSMTPCommand(ssl, emailContent, "250");

        // Quit
        sendSMTPCommand(ssl, "QUIT\r\n", "221");

        // Cleanup
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();

        std::cout << "Email sent successfully.\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        WSACleanup();
        return 1;
    }

    return 0;
}
