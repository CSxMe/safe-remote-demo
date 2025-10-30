#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>


class SimpleClient {
public:
    
    SimpleClient(const std::string& serverIp, int serverPort, const std::string& authToken);

    
    ~SimpleClient();

  
    bool connectAndAuthenticate();

  
    std::string sendCommandAndGetResponse(const std::string& command);

    
    void closeConnection();

    bool isConnected() const;

private:
    bool sendStr(const std::string& msg);
    std::string recvStr();

private:
    std::string m_serverIp;
    int m_serverPort;
    std::string m_authToken;
    SOCKET m_socket;
    bool m_connected;
};

#endif 
