#pragma once
#include <string>
#include <cstring>
#include <iostream>
class Buffer
{
private:
    std::string buf_;       //用于存放数据，既可以存放文本数据，也可以存放二进制数据
    const uint16_t sep_;    //报文的分隔符，0-无分隔符; 1-四字节的报头; 2-"\r\n\r\n"分隔符(http)协议

public:
    Buffer(uint16_t sep = 1);
    ~Buffer();

    void append(const char *data,size_t size);      //将数据追加到Buffer中
    void appendwithsep(const char *data,size_t size);//将数据追加到Buffer中，附加报文头部
    void erase(size_t pos,size_t nn);               //从buf_的pos开始，删除nn个字节，pos从0开始
    size_t size();      //返回buf_大小
    const char *data(); //返回buf_首地址
    void clear();       //清空bUf_
    bool pickmessage(std::string &ss);  //从buf_中拆分出一个报文，存放在ss中，如果没有报文，返回false

};

