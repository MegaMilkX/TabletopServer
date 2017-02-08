#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <iostream>
#include <string>
#include <vector>
#include <map>

#define ROK 0

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> result;
    std::string s = str;
    
    size_t start = 0U;
    size_t end = s.find(delim);
    while(end != std::string::npos)
    {
        result.push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
        if(end == std::string::npos)
            result.push_back(s.substr(start, end));
    }
    
    return result;
}

class HTTPRequest
{
public:    
    int FromString(const std::string& str)
    {
        std::string endStr = "\r\n\r\n";
        size_t endPos = str.find(endStr);
        if(endPos == std::string::npos)
            return -1;
        
        std::vector<std::string> lines = split(str, "\r\n");
        
        if(lines.size() == 0)
            return -2;
        
        std::vector<std::string> requestLine = split(lines[0], " ");
        
        if(requestLine.size() != 3)
            return -3;
        method = requestLine[0];
        uri = requestLine[1];
        http = requestLine[2];
        
        for(unsigned i = 1; i < lines.size(); ++i)
        {
            std::string delim = ": ";
            size_t pos = lines[i].find(delim);
            if(pos == std::string::npos)
                continue;
            std::string headerName = lines[i].substr(0, pos);
            std::string headerContent = lines[i].substr(pos + delim.size(), std::string::npos);
            headers.insert(std::make_pair(headerName, headerContent));
        }
        
        return endPos + endStr.size();
    }
    
    void Print()
    {
        std::cout << "Method: " << method << std::endl;
        std::cout << "URI: " << uri << std::endl;
        std::cout << "VER: " << http << std::endl;
        
        std::map<std::string, std::string>::iterator it = headers.begin();
        for(it; it != headers.end(); ++it)
        {
            std::cout << it->first << " " << it->second << std::endl;
        }
    }
    
    std::string Method() { return method; }
    std::string URI() { return uri; }
private:
    std::string method;
    std::string uri;
    std::string http;
    std::map<std::string, std::string> headers;
};

class HTTPResponse
{
public:
    HTTPResponse()
    : version("HTTP/1.1"),
    status("200"),
    message("OK")
    {}
    
    operator std::string()
    {
        std::string response;
        response += version + " ";
        response += status + " ";
        response += message + "\r\n";
        
        std::map<std::string, std::string>::iterator it = headers.begin();
        for(it; it != headers.end(); ++it)
        {
            response += it->first + ": ";
            response += it->second + "\r\n";
        }
        
        response += "\r\n";
        response += data;
        
        return response;
    }
    
    void Version(const std::string& str) { version = str; }
    void Status(const std::string& str) { status = str; }
    void Message(const std::string& str) { message = str; }
    void Header(const std::string& header, const std::string& value) { headers[header] = value; }
    void Data(const std::string& data)
    {
        this->data = data;
        Header("Content-Length", std::to_string(data.size()));
    }
private:
    std::string version;
    std::string status;
    std::string message;
    std::string data;
    std::map<std::string, std::string> headers;
};

class HTTPModule
{
public:
    virtual ~HTTPModule() {}
    virtual std::string HTML() = 0;
};

class HTTPServer
{
public:
    int Init()
    {
        if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            return 1;
        
        if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
            return 1;
        
        srv.sin_family = AF_INET;
        srv.sin_addr.s_addr = INADDR_ANY;
        srv.sin_port = htons(80);
        
        if(bind(masterSocket, (sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR)
            return 1;
        
        return ROK;
    }
    
    void Cleanup()
    {
        closesocket(masterSocket);
        WSACleanup();
    }
    
    int Run()
    {
        int c = 0;
        while(!listen(masterSocket, 3))
        {
            c = sizeof(sockaddr_in);
            newSocket = accept(masterSocket, (sockaddr*)&client, &c);
            if(newSocket == INVALID_SOCKET)
                continue;
            
            HTTPRequest request;
            char* buf = new char[512];
            std::string data = leftoverData;
            size_t dataRead = 0;
            
            do
            {
                size_t received = recv(newSocket, buf, 512, 0);
                data += std::string(buf, buf + received);
            }
            while((dataRead = request.FromString(data)) < 0);
            leftoverData = data.substr(dataRead, std::string::npos);
            
            request.Print();
            
            if(modules[request.URI()] != 0)
            {
                std::string html = modules[request.URI()]->HTML();
                HTTPResponse response;
                response.Header("Server", "D&D");
                response.Header("Content-Type", "text/html; charset=UTF-8");
                response.Data(html);
                
                std::string resp = response;
                
                send(newSocket, resp.c_str(), resp.size(), 0);
                
                std::cout << resp << std::endl;
            }
            else
            {
                HTTPResponse response;
                response.Status("404");
                std::string resp = response;
                send(newSocket, resp.c_str(), resp.size(), 0);
            }
            
            delete[] buf;
            
            closesocket(newSocket);
        }
        
        return 0;
    }
    
    template<typename T>
    void AddModule(const std::string& path)
    {
        std::map<std::string, HTTPModule*>::iterator it = modules.find(path);
        if(it != modules.end())
            if(it->second != 0)
                delete it->second;
        modules.insert(std::make_pair(path, new T));
    }
private:
    WSADATA wsa;
    SOCKET masterSocket, newSocket;
    sockaddr_in srv, client;
    std::string leftoverData;
    
    std::map<std::string, HTTPModule*> modules;
};

class IndexPage : public HTTPModule
{
public:
    std::string HTML()
    {
        return R"(
            <!DOCTYPE html>
            <html>
            <head>
            <meta charset="UTF-8">
            <title>Title of the document</title>
            </head>

            <body>
            Content of the document......
            </body>

            </html> 
        )";
    }
};

int main()
{
    HTTPServer server;
    server.Init();
    server.AddModule<IndexPage>("/");
    
    server.Run();
    
    server.Cleanup();
    return 0;
}