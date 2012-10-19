#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <iostream>

#include "Channel.h"
#include "Exceptions.h"

namespace IPC{

void defaultDeallocator(void *, void *);

class ISocket{
  public:
    virtual ~ISocket(){}
    virtual void send(const void *buffer, size_t size, void *hint = NULL) = 0;
    virtual size_t receive(void **buffer, size_t size = 0) = 0;
};

}

#endif
