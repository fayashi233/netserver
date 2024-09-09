#include "Buffer.h"

Buffer::Buffer(uint16_t sep):sep_(sep)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data,size_t size)      //将数据追加到Buffer中
{
    buf_.append(data,size);
}

void Buffer::appendwithsep(const char *data,size_t size)//将数据追加到Buffer中，附加报文分隔符
{
    if(sep_==0)
    {
        buf_.append(data,size);
    }else if(sep_==1)
    {
        buf_.append((char*)&size,4);
        buf_.append(data,size);
    }else if(sep_==2)
    {
        //buf_.append((char*)&size,4);
        buf_.append(data,size);
        buf_.append("\r\n\r\n");
    }

}

void Buffer::erase(size_t pos,size_t nn)               //从buf_的pos开始，删除nn个字节，pos从0开始
{
    buf_.erase(pos,nn);
}


size_t Buffer::size()      //返回buf_大小
{
    return buf_.size();
}

const char *Buffer::data() //返回buf_首地址
{
    return buf_.data();
}


void Buffer::clear()       //清空bUf_
{
    buf_.clear();
}


bool Buffer::pickmessage(std::string &ss)  //从buf_中拆分出一个报文，存放在ss中，如果没有报文，返回false
{
    if(buf_.size()==0) return false;
    if(sep_==0)
    {
        ss = buf_;
        buf_.clear();
    }
    else if(sep_==1)
    {
        int len;
        memcpy(&len,buf_.data(),4);
        if(buf_.size()<len+4) return false;        //如果接收缓冲区中的数据量少于报文头部，说明接收缓冲区的报文内容不完整
        ss=buf_.substr(4,len);
        buf_.erase(0,len+4);                //从inputbuffer中删除已经获取的报文
    }
    else if(sep_==2)
    {
        size_t pos = buf_.find("\r\n\r\n");
        if (pos == std::string::npos) return false;  
        ss = buf_.substr(0, pos);
        buf_.erase(0, pos + 4);  
    }else
    {
        return false;
    }
    return true;
}