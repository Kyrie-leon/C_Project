#pragma once
#include<unistd.h>
#include"Sock.hpp"

#define PORT 8081

class HttpServer
{
  private:
    int port;
    int lsock;
  public:
    //构造
    HttpServer(int _p = PORT)
      :port(_p),lsock(-1)
    {}
    
    void InitServer()
    {
      lsock = Sock::Socket();
      Sock::Bind(lsock, port);
      Sock::Listen(lsock);

    }
    
    void Start()
    {
      struct sockaddr_in endpoint;
      for(;;)
      {
        int sock = Sock::Accept(lsock);
        if(sock < 0)
        {
          continue;
        }

        pthread_t tid;
        int *sock
      }
    }

    //析构
    ~HttpServer()
    {
      if(lsock >= 0)
      {
        close(lsock);
      }
    }

};
