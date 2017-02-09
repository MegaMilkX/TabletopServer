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
        
        if(method == "GET")
        {
            size_t pos = uri.find('?');
            if(pos != std::string::npos)
            {
                std::vector<std::string> prts = split(uri, "?");
                uri = prts[0];
                ParseParams(prts[1]);
            }
        }
        
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
    
    std::string Header(const std::string& header) { return headers[header]; }
    std::map<std::string, std::string> Params() { return params; }
    std::string Method() { return method; }
    std::string URI() { return uri; }
private:
    void ParseParams(const std::string& str)
    {
        std::vector<std::string> p = split(str, "&");
        if(p.empty())
            p.push_back(str);
        
        for(unsigned i = 0; i < p.size(); ++i)
        {
            std::vector<std::string> kv = split(p[i], "=");
            if(kv.empty())
                params.insert(std::make_pair(p[i], ""));
            else
                params.insert(std::make_pair(kv[0], kv[1]));
        }
    }

    std::string method;
    std::string uri;
    std::string http;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
};

#endif
