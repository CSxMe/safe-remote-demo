// client.cpp
#include "client.h"
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

// ---------- exe SimpleClient ----------
SimpleClient::SimpleClient(const std::string& serverIp, int serverPort, const std::string& authToken)
    : m_serverIp(serverIp), m_serverPort(serverPort), m_authToken(authToken),
    m_socket(INVALID_SOCKET), m_connected(false)
{
}

SimpleClient::~SimpleClient() {
    closeConnection();
}

bool SimpleClient::isConnected() const {
    return m_connected;
}

bool SimpleClient::sendStr(const std::string& msg) {
    if (!m_connected) return false;
    int len = static_cast<int>(msg.size());
    int sent = send(m_socket, reinterpret_cast<const char*>(&len), sizeof(len), 0);
    if (sent != sizeof(len)) return false;

    const char* data = msg.data();
    int total = 0;
    while (total < len) {
        int n = send(m_socket, data + total, len - total, 0);
        if (n == SOCKET_ERROR || n == 0) return false;
        total += n;
    }
    return true;
}

std::string SimpleClient::recvStr() {
    if (!m_connected) return std::string();
    int len = 0;
    int r = recv(m_socket, reinterpret_cast<char*>(&len), sizeof(len), 0);
    if (r <= 0) return std::string();
    if (len <= 0) return std::string();

    std::string buf;
    buf.resize(len);
    int got = 0;
    while (got < len) {
        int n = recv(m_socket, &buf[got], len - got, 0);
        if (n <= 0) return std::string();
        got += n;
    }
    return buf;
}

bool SimpleClient::connectAndAuthenticate() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, m_serverIp.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(static_cast<uint16_t>(m_serverPort));

    if (connect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "connect() failed (server may be unreachable)\n";
        closesocket(m_socket);
        WSACleanup();
        return false;
    }

    m_connected = true;

    if (!sendStr(m_authToken)) {
        std::cerr << "Failed to send auth token\n";
        closeConnection();
        return false;
    }

    std::string resp = recvStr();
    if (resp != "AUTH_OK") {
        std::cerr << "Authentication failed: " << resp << "\n";
        closeConnection();
        return false;
    }

    return true;
}

std::string SimpleClient::sendCommandAndGetResponse(const std::string& command) {
    if (!m_connected) return std::string();
    if (!sendStr(command)) {
        std::cerr << "Failed to send command\n";
        closeConnection();
        return std::string();
    }
    std::string res = recvStr();
    if (res.empty()) {
        closeConnection();
    }
    return res;
}

void SimpleClient::closeConnection() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    if (m_connected) m_connected = false;
    WSACleanup();
}
// ---------- end exe =  SimpleClient ----------
