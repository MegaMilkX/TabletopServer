#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <fstream>

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
            
            char* buf = new char[512];
    
            HTTPRequest request;
            
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
            
            HTTPResponse response;
            response.Header("Server", "D&D");
            
            if(modules[request.URI()] != 0)
            {
                modules[request.URI()]->Params(request.Params());
                
                std::string html = modules[request.URI()]->HTML();
                
                response.Header("Content-Type", "text/html; charset=UTF-8");
                response.Data(html);
                
                std::string resp = response;
                
                Send(newSocket, resp.c_str(), resp.size());
                
                
            }
            else
            {
                std::ifstream file;
                std::string filePath = replace_char(request.URI(), '/', '\\');
                if(filePath[0] == '\\')
                    filePath.erase(filePath.begin());
                file.open(filePath, std::ios::binary | std::ios::ate);
                
                if(!file)
                {
                    response.Status("404");
                    Send(newSocket, response);
                }
                else
                {
                    std::streamsize size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    std::vector<char> buffer(size);
                    file.read(buffer.data(), size);
                    
                    std::string dataStr(buffer.begin(), buffer.end());
                    response.Data(dataStr);
                    response.Header("Content-Type", GetMIMEType(request.URI()));
                    Send(newSocket, response);
                }
            }
            
            //std::cout << (std::string)response << std::endl;
            
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
    
    void AddMIME(const std::string& extension, const std::string& mime)
    {
        mimeTypes[extension] = mime;
    }
    
private:
    void Send(SOCKET sock, const HTTPResponse& response)
    {
        std::string resp = response;
        Send(sock, resp.c_str(), resp.size());
    }

    void Send(SOCKET sock, const char* ptr, size_t len)
    {
        int sent = 0;
        while((unsigned int)sent < len)
        {
            sent = send(sock, ptr + sent, len - sent, 0);
            if(sent == SOCKET_ERROR)
                break;
        }
    }
    
    std::string GetMIMEType(const std::string& fileName)
    {
        std::map<std::string, std::string>::iterator it = mimeTypes.find(FileNameExtension(fileName));
        if(it == mimeTypes.end())
            return "application/octet-stream";
        else
            return it->second;
    }

    WSADATA wsa;
    SOCKET masterSocket, newSocket;
    sockaddr_in srv, client;
    std::string leftoverData;
    
    std::map<std::string, HTTPModule*> modules;
    std::map<std::string, std::string> mimeTypes;
};

#endif
