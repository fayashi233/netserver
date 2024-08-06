#include "Buffer.h"

Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data,size_t size)      //将数据追加到Buffer中
{
    buf_.append(data,size);
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
