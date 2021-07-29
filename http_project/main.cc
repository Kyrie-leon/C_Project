#include"HttpServer.hpp"
#include"Log.hpp"

int main(int argc, char* argv[])
{
  HttpServer* svr = new HttpServer();
  svr->InitServer();
  svr->Start();

  return 0;
}
