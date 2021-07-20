#pragma once
#include<iostream>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"Log.hpp"

#define BACKLOG 5

class Sock
{
  public:
    static int Socket()
    {
      int sock = socket(AF_INET, SOCK_STREAM, 0);
      if(sock < 0)
      {
        LOG(Fatal,"socket creat error");
        exit(SocketErr);
      }
      return sock;
    }

    static void Bind(int lsock, int port)
    {
      //填充结构体协议
      struct sockaddr_in local;
      bzero(&local, sizeof(local));
      local.sin_family = AF_INET;
      local.sin_addr.s_addr = htonl(INADDR_ANY);
      local.sin_port = htons(port);
      if(bind(lsock, (struct sockaddr* )&local, sizeof(local)) < 0 )
      {
        LOG(Fatal, "bind error");
        exit(BindErr);
      }
    }

    static void Listen(int sock)
    {
      if(listen(sock, BACKLOG) < 0)
      {
        LOG(Fatal, "listen error");
        exit(ListenErr);
      }
    }

    static void SetSockOpt(int sock)
    {
      int opt = 1;
      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    static int Accept(int lsock)
    {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);
      int s = accept(lsock, (struct sockaddr *)&peer, &len);
      if(s < 0)
      {
        LOG(Warning, "accetp error");
      }

      return s;
    }
};
