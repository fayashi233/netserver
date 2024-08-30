#include "Timestamp.h"

Timestamp::Timestamp()
{
    secsinceepoch_ = time(0);   //取系统当前时间
}

Timestamp::Timestamp(int64_t secsinceepoch):secsinceepoch_(secsinceepoch)   //用一个整数初始化对象
{

}
Timestamp::~Timestamp()
{
}

Timestamp Timestamp::now()     //返回当前时间的Timestamp对象
{
    return Timestamp();
}
time_t Timestamp::toint() const   //返回整数表示的时间
{
    return secsinceepoch_;
}
std::string Timestamp::tostring() const   //返回字符串表示的时间，格式：yyyy-mm-dd hh24:mi:ss
{
    char buf[32] = {0};
    tm *tm_time = localtime(&secsinceepoch_);
    snprintf(buf,32,"%4d-%02d-%02d %02d:%02d:%02d",
        tm_time->tm_year + 1900,
        tm_time->tm_mon + 1,
        tm_time->tm_mday,
        tm_time->tm_hour,
        tm_time->tm_min,
        tm_time->tm_sec
    );
    return buf;
}


// #include <unistd.h>
// #include <iostream>

// int main()
// {
//     Timestamp ts;
//     std::cout<<ts.toint()<<std::endl;
//     std::cout<<ts.tostring()<<std::endl;
//     sleep(1);
//     std::cout<<Timestamp::now().toint()<<std::endl;
//     std::cout<<Timestamp::now().tostring()<<std::endl;
// }