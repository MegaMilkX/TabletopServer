#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>
#include <iostream>

#include "util.h"

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

#endif
