#pragma once
#include"Sock.hpp"
#include"Log.hpp"

class Entry{
  public:
    //HandlerHttp处理函数
    // 1.sock解引用
    static void *HandlerHttp(void* arg)
    {
      int sock = *(int*)arg;
      delete (int*)arg;
//测试代码显示http请求
#ifdef DEBUG
    char request[10240];
    recv(sock, request, sizeof(request), 0);
    std::cout << request << std::endl;
    close(sock);
#else
    std::cout<< "other code.." << std::endl;
#endif
    }
};
