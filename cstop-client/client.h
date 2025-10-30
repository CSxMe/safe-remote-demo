#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>

// SimpleClient: يقوم بفتح اتصال TCP مع سيرفر خارجي، إرسال "command" كنص، 
// واستقبال الاستجابة كنص. لا ينفذ أوامر على الجهاز، فقط إرسال/استقبال نصوص.
// الاستخدام: main.cpp ينشئ SimpleClient، يستدعي connectAndAuthenticate() ثم
// يستعمل sendCommandAndGetResponse() لكل أمر.
class SimpleClient {
public:
    // constructor: serverIp (مثال "127.0.0.1"), serverPort (مثال 5000), authToken (سلسلة مصادقة بسيطة)
    SimpleClient(const std::string& serverIp, int serverPort, const std::string& authToken);

    // destructor يغلق الاتصال تلقائياً
    ~SimpleClient();

    // يفتح الاتصال ويفعل المصادقة (يرجع true لو نجح)
    bool connectAndAuthenticate();

    // يرسل الأمر ويرجع الرد من السيرفر كسلسلة
    // لو حدث خطأ أو انقطع الاتصال يعيد empty string
    std::string sendCommandAndGetResponse(const std::string& command);

    // يغلق الاتصال يدوياً
    void closeConnection();

    // هل متصل الآن؟
    bool isConnected() const;

private:
    // دوال مساعدة لإرسال واستقبال نصوص (طول + بايتات)
    bool sendStr(const std::string& msg);
    std::string recvStr();

private:
    std::string m_serverIp;
    int m_serverPort;
    std::string m_authToken;
    SOCKET m_socket;
    bool m_connected;
};

#endif // CLIENT_H
