#pragma once
#include"Sock.hpp"
#include"Log.hpp"
#include"Util.hpp"

#define WEBROOT "wwwroot"
#define HOMEPAGE "predict.html"
#define VERSION "HTTP/1.0"

static std::string CodeToDesc(int code)
{
  std::string desc;
  switch(code){
    case 200:
      desc = "OK";
      break;
    case 404:
      desc = "Not Found";
      break;
    default:
      desc = "OK";
      break;
    }
  return desc;
}

//http请求类
class HttpRequest{
  private:
    std::string request_line; //请求行
    std::string method;
    std::string uri;
    std::string version;
    std::vector<std::string> request_header;  //请求头
    ssize_t content_length;
    std::unordered_map<std::string, std::string> header_kv;
    std::string blank;  //空行
    std::string request_body; //请求体
    std::string path; //web路径
    std::string query_string;
    bool cgi;
    ssize_t file_size;
    std::string suffix;

  public:
    HttpRequest()
      :blank("\n"),content_length(-1),path(WEBROOT),cgi(false), suffix("text/html")
    {}
    void SetRequestLine(const std::string line)
    {
      request_line = line;
    }

    void RequestLineParse()
    {
      Util::StringParse(request_line, method, uri, version);
      LOG(Notice, request_line);
      LOG(Notice, method);
      LOG(Notice, uri);
      LOG(Notice, version);

    }
    void InsertHeaderLine(const std::string &line)
    {
      request_header.push_back(line);
      LOG(Notice, line);
    }
    
    void RequestHeaderParse()
    {
      for(auto& e:request_header)
      {
        std::string k,v;
        Util::MakeStringToKV(e, k, v);
        LOG(Notice, k);
        LOG(Notice, v);
        if(k == "Content-Length"){
          content_length = Util::StringToInt(v);
        }
        header_kv.insert({k,v});
      }
    }

    bool IsNeedRecvBody()
    {
      //Post PoSt, POST需要不区分大小写
      if(strcasecmp(method.c_str(), "post") == 0 && content_length > 0)
      {
        return true;
      }
      return false;
    }

    ssize_t GetContenLength()
    {
      return content_length;
    }

    void SetRequestBody(std::string body)
    {
      request_body = body;
    }
    
    bool IsMethodLegal()
    {
      if(strcasecmp(method.c_str(), "POST") == 0 || strcasecmp(method.c_str(), "GET") == 0){
        return true;
      }
      return false;
    }

    bool IsGet()
    {
      return strcasecmp(method.c_str(), "GET")==0 ? true :false;
    }
    
    bool IsPost()
    {
      return strcasecmp(method.c_str(), "POST")==0 ? true :false;
    }
    
    bool IsCgi()
    {
      return cgi;
    }

    void SetCgi()
    {
      cgi = true;
    }
    void UriParse()
    {
      //uri存在
      std::size_t pos = uri.find("?");
      // ?
      if(pos == std::string::npos){
        path += uri;
      }
      else{
        path += uri.substr(0, pos);
        query_string = uri.substr(pos+1);
        cgi = true;
      }
    }

    void SetUriEqPath()
    {
      path += uri;
    }

    void IsAddHomePage()
    {
      //path  or  /a/b/c.htm  or /s
      if(path[path.size()-1] == '/'){
        path += HOMEPAGE;
      }
    }
    
    std::string GetPath()
    {
      return path;
    }
    
    std::string SetPath(std::string _path)
    {
      path = _path;
    }
    

    void SetFileSize(ssize_t s)
    {
      file_size = s;
    }
    
    ssize_t GetFileSize()
    {
      return file_size;
    }
};

//http响应类
class HttpResponse{
  private:
    std::string status_line;
    std::vector<std::string> response_header;
    std::string blank;
    std::string response_body;
  public:
    HttpResponse():blank("\n")
    {}
    
    void SetStatusLine(const std::string& line)
    {
      status_line = line;
    }

    std::string GetStatusLine()
    {
      return status_line;
    }

    std::vector<std::string> GetRspHeader()
    {
      response_header.push_back(blank);
      return response_header;
    }

    ~HttpResponse()
    {}
};

class EndPoint{
  private:
    int sock; //套接字文件描述符
    HttpRequest req;
    HttpResponse rsp;
  private:
    void GetRequestLine()
    {
      std::string line;
      Sock::Getline(sock, line);
      req.SetRequestLine(line);
      req.RequestLineParse();

    }
    void GetRequestHeader()
    {
      //Http求情按行读取存储
      std::string line;
      do{
          line = "";
          Sock::Getline(sock, line);
          req.InsertHeaderLine(line);
      }while(!line.empty());
      //请求头分离
      req.RequestHeaderParse();
    }

    void GetRequestBody()
    {
      ssize_t len = req.GetContenLength();
      char c;
      std::string body;
      //将len个字节长度的内容读到body中
      while(len--)
      {
        ssize_t s = recv(sock, &c, 1, 0);
        body.push_back(c);
      }
      req.SetRequestBody(body);
    }
  public:
    EndPoint(int _sock):sock(_sock)
    {}

    void RecvRequest()
    {
      //获取完整http请求
      //将http报文分解为报头，请求体，正文
      //读取并分析第一行
      GetRequestLine();
      //读取报头
      GetRequestHeader();
      //是否需要读取正文
      if(req.IsNeedRecvBody())
      {
        GetRequestBody();
      }
      //到这里读完了所有请求
    }

    void SetResponseStatusLine(int code)
    {
      std::string status_line;
      status_line += VERSION;
      status_line += " ";
      status_line += std::to_string(code);
      status_line += " ";
      status_line += CodeToDesc(code);
      status_line += "\r\n";
      
      rsp.SetStatusLine(status_line);
    }

    void SetResponseHeaderLine()
    {

    }

    void MakeResponse()
    {
      //分析http请求
      //GET POST
      int code = 200;
      ssize_t size = 0; //filesize
      std::string path;
      if(!req.IsMethodLegal())
      {
        LOG(Warning, "method is illegal");
        code = 404;
        goto end;
      }
      if(req.IsGet())
      {
        req.UriParse();
      }
      else{
        //Post方法
        req.SetUriEqPath();
      }
      req.IsAddHomePage();
      //get && 没有参数 ->path
      //get && 有参数 -> path && query_string
      //Post -> uri ->path && body
      path = req.GetPath();
      LOG(Notice, path);
      struct stat st;
      //判断文件权限
      if(stat(path.c_str(), &st) < 0){
        LOG(Warning, "html is not exist!404");
        code =404;
        goto end;
      }
      else{
        if(S_ISDIR(st.st_mode)){
          path += "/";
          req.SetPath(path);
          req.IsAddHomePage();
        }
        else{
          if((st.st_mode & S_IXUSR) ||\
             (st.st_mode & S_IXGRP) ||\
             (st.st_mode & S_IXOTH)){
              req.SetCgi();
          }
          else{
            //正常的网页请求
          }
        }
        if(!req.IsCgi()){
          req.SetFileSize(st.st_size);
        }
      }
end:
      //制作response
      SetResponseStatusLine(code);
      SetResponseHeaderLine();
      //TODO
    }

    void ExecNonCgi(const std::string path)
    {
      ssize_t size = req.GetFileSize();
      int fd = open(path.c_str(), O_RDONLY);
      if(fd < 0)
      {
        LOG(Error, "path is not exist bug!!");
        return;
      }
      sendfile(sock, fd, nullptr, size);
      close(fd);
    }

    void ExecCgi()
    {

    }

    void SendResponse()
    {
      //状态行 响应报头
      std::string line = rsp.GetStatusLine();
      send(sock, line.c_str(), line.size(), 0);
      const auto &header = rsp.GetRspHeader();
      auto it = header.begin();
      while(it != header.end())
      {
        send(sock, it->c_str(), it->size(), 0);
        it++;
      }

      if(req.IsCgi())
      {
        LOG(Notice, "use cgi model!");
        ExecCgi();
      }
      else{
        LOG(Notice, "use non-cgi model!");
        std::string  path = req.GetPath();
        ExecNonCgi(path);
      }
    }
};

//http
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
    EndPoint* ep = new EndPoint(sock);
    ep->RecvRequest();  //接收远端http请求
    ep->MakeResponse(); //制作http响应
    ep->SendResponse(); //发送http响应
#endif
    }
};
