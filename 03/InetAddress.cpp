#include "InetAddress.h"
// class InetAddress
// {
// private:
//     sockaddr_in addr_;

// public:
//     InetAddress(const std::string &ip, uint16_t port);   // 监听的fd，用这个构造函数
//     InetAddress(const sockaddr_in addr) : addr_(addr) {} // 如果是客户端连上来的fd，用这个函数
//     ~InetAddress();

//     const char *ip() const;       // 返回字符串表示的地址  192.168.28.128
//     uint16_t port() const;        // 可以从常量对象调用   返回整数表示的端口 8080
//     const sockaddr *addr() const; // 返回addr_成员
// };


InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());      // 服务端用于监听的ip地址。
    addr_.sin_port = htons(port);               // 服务端用于监听的端口。
}

InetAddress::~InetAddress()
{

}

const char *InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr *InetAddress::addr() const
{
    return (sockaddr*)&addr_;
}

InetAddress::InetAddress(const sockaddr_in addr) : addr_(addr) 
{

}

void InetAddress::setaddr(sockaddr_in clientaddr)
{
    addr_ = clientaddr;
}
