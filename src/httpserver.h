#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <fstream>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

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
        std::map<std::string, HTTPRequestHandler*>::iterator it = handlers.begin();
        for(it; it != handlers.end(); ++it)
        {
            if(it->second != 0)
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
            int dataRead = 0;
            int received = 0;
            do
            {
                int received = recv(newSocket, buf, 512, 0);
                if(received == SOCKET_ERROR || !received)
                    break;
                data += std::string(buf, buf + received);
            }
            while((dataRead = request.FromString(data)) < 0);
            
            if(received == SOCKET_ERROR)
                continue;
            
            if(dataRead > 0)
                leftoverData = data.substr(dataRead - 1, std::string::npos);
            request.Print();
            
            HTTPResponse response;
            
            if(handlers[request.URI()] != 0)
            {
                (*handlers[request.URI()])(request, response);
            }
            else
            {
                std::ifstream file;
                std::string filePath = replace_char(request.URI(), '/', '\\');
                if(filePath[0] == '\\')
                    filePath.erase(filePath.begin());
                
                std::map<std::string, std::string>::iterator it =
                    mimeTypes.find(FileNameExtension(filePath));
                    
                if(it == mimeTypes.end())
                {
                    response.Status("404");
                }
                else
                {
                    file.open(filePath, std::ios::binary | std::ios::ate);
                
                    if(!file)
                    {
                        response.Status("404");
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
                        
                        file.close();
                    }
                }
            }
            
            Send(newSocket, response);
            
            delete[] buf;
            closesocket(newSocket);
        }
        
        return 0;
    }
    
    template<typename T>
    void AddHandler(const std::string& path)
    {
        std::map<std::string, HTTPRequestHandler*>::iterator it = handlers.find(path);
        if(it != handlers.end())
            if(it->second != 0)
                delete it->second;
        handlers.insert(std::make_pair(path, new T));
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
    
    std::map<std::string, HTTPRequestHandler*> handlers;
    std::map<std::string, std::string> mimeTypes;
};

#endif
