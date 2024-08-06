#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>


//socket的地址协议类
class InetAddress
{
private:
    sockaddr_in addr_;

public:
    InetAddress();
    InetAddress(const std::string &ip, uint16_t port);   // 监听的fd，用这个构造函数
    InetAddress(const sockaddr_in addr); // 如果是客户端连上来的fd，用这个函数
    ~InetAddress();

    const char *ip() const;       // 返回字符串表示的地址  192.168.28.128
    uint16_t port() const;        // 可以从常量对象调用   返回整数表示的端口 8080
    const sockaddr *addr() const; // 返回addr_成员
    void setaddr(sockaddr_in clientaddr); //设置addr_成员的值
};