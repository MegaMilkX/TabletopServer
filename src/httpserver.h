#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#include "httprequest.h"
#include "httpresponse.h"
#include "httpmodule.h"

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
        std::map<std::string, HTTPModule*>::iterator it = modules.begin();
        for(it; it != modules.end(); ++it)
        {
            delete it->second;
        }
        
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

#endif
