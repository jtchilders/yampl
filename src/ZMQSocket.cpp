#include "ZMQSocket.h"

using namespace IPC;

ZMQBaseSocket::ZMQBaseSocket(Channel channel, int type, bool ownership, void (*deallocator)(void *, void *)) : m_channel(channel), m_ownership(ownership), m_deallocator(deallocator){
  if(m_channel.topology == MANY_TO_MANY)
    throw UnsupportedException();

  Topology topo = m_channel.topology;
  m_socket = new zmq::socket_t(m_channel.environment.getContext(), type);
 
  if(type == ZMQ_PUSH) 
    m_socket->setsockopt(ZMQ_HWM, &m_channel.asynchronicity, sizeof(m_channel.asynchronicity));

  if((topo == ONE_TO_ONE || topo == ONE_TO_MANY) && (type == ZMQ_PUSH || type == ZMQ_REQ))
    m_socket->bind(m_channel.name.c_str());
  else if(topo == MANY_TO_ONE && type == ZMQ_REP)
    m_socket->bind(m_channel.name.c_str());
  else
    m_socket->connect(m_channel.name.c_str());
}

void ZMQBaseSocket::send(const void *buffer, size_t size, void *hint){
  if(m_ownership){
    zmq::message_t message((void *)buffer, size, m_deallocator, hint);
    m_socket->send(message);
  }else{
    zmq::message_t message(size);
    memcpy((void*)message.data(), buffer, size);
    m_socket->send(message);
  }
}

int ZMQBaseSocket::receive(void **buffer, size_t size){
  static zmq::message_t message;
  static bool received = false;

  if(!received){
    m_socket->recv(&message);
    received = true;
  }

  if(m_ownership){
    *buffer = message.data();
  }else{
    if(*buffer && size < message.size())
      throw InvalidSizeException();
    else if(*buffer)
      memcpy(*buffer, message.data(), message.size());
    else{
      *buffer = malloc(message.size());
      memcpy(*buffer, message.data(), message.size());
    }
  }

  received = false;
  return message.size();
}
