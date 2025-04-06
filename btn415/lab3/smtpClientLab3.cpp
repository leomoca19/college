#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "base64.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

std::string getFileContent(const char* fname){
    std::ifstream file(fname, std::ios::binary);
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    return content.str();
}

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
    const std::string emailSender = "leomoca19@gmail.com";
    const std::string emailPassword = "bztxvrmbwripjfml";
    const std::string emailReceiver = "leomoca19@gmail.com";

    const std::string subject = "Test Email from Winsock C++";
    const std::string body = "Test email with attachments";

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

        // Read and encode attachment from file
        const char* fname = "email-attach.txt";
        std::string fileData = getFileContent(fname);
        std::string fileB64 = base64_encode(reinterpret_cast<const unsigned char*>(fileData.c_str()), fileData.size());

        sendSMTPCommand(ssl, base64Email + "\r\n", "334");
        sendSMTPCommand(ssl, base64Password + "\r\n", "235");

        // Splitting file's data into 76 chars because MIME requires this length to interpret data correctly
        std::string attachment;
        for (size_t i = 0; i < fileB64.size(); i += 76)
            attachment += fileB64.substr(i, 76) += "\r\n\r\n";

        // attachment MIME formatted
        const char* boundary = "------------";
        std::ostringstream attachmentMIME;
        attachmentMIME
        << "--" << boundary << "\r\n"
        << "Content-Type: application/octet-stream; name=\"" << fname << "\"\r\n" 
        << "Content-Transfer-Encoding: base64\r\n"
        << "Content-Disposition: attachment; filename=\"" << fname << "\"\r\n\r\n"
        << attachment << "\r\n"
        << "--" << boundary << "--\r\n";


        // Compose email's "meat" following MIME's format
        std::string emailContent = 
            // declare headers and multipart
            "Subject: " + subject + "\r\n" 
            "MIME-Version: 1.0\r\n"
            "Content-Type: multipart/mixed; boundary=\"" + boundary + "\"\r\n\r\n"
            // declare text type
            "--" + boundary + "\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "Content-Transfer-Encoding: 7bit\r\n\r\n"
            + body + "\r\n\r\n"
            + attachmentMIME.str()
            // end of data
            + ".\r\n";
        
        // Send email
        sendSMTPCommand(ssl, "MAIL FROM:<" + emailSender + ">\r\n", "250");
        sendSMTPCommand(ssl, "RCPT TO:<" + emailReceiver + ">\r\n", "250");
        sendSMTPCommand(ssl, "DATA\r\n", "354");
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
