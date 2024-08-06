#pragma once
#include <string>
#include <iostream>
class Buffer
{
private:
    std::string buf_;       //用于存放数据，既可以存放文本数据，也可以存放二进制数据

public:
    Buffer();
    ~Buffer();

    void append(const char *data,size_t size);      //将数据追加到Buffer中
    size_t size();      //返回buf_大小
    const char *data(); //返回buf_首地址
    void clear();       //清空bUf_

};

