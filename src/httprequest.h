#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>
#include <iostream>
#include <stdlib.h>
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
        
        std::vector<std::string> lines = util::split(str, "\r\n");
        
        if(lines.size() == 0)
            return -2;
        
        std::vector<std::string> requestLine = util::split(lines[0], " ");
        
        if(requestLine.size() != 3)
            return -3;
        method = requestLine[0];
        uri = requestLine[1];
        path = requestLine[1];
        
        std::string::size_type pos = uri.find('?');
        if(pos != std::string::npos)
        {
            std::vector<std::string> prts = util::split(uri, "?");
            path = prts[0];
            if(method == "GET")
                ParseParams(prts[1]);
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
        
        unsigned int contentLen = Header<unsigned int>("Content-Length");
        if(str.size() < endPos + endStr.size() + contentLen)
            return -4;
        std::string content = str.substr(endPos + endStr.size(), endPos + endStr.size() + contentLen);
        
        if(method == "POST")
        {
            std::string contentType = Header<std::string>("Content-Type");
            if(contentType.find("application/x-www-form-urlencoded") != std::string::npos)
            {
                ParseParams(content);
            }
        }
        
        return endPos + endStr.size() + contentLen;
    }
    
    void Print() const
    {
        std::cout << "Method: " << method << std::endl;
        std::cout << "URI: " << uri << std::endl;
        std::cout << "VER: " << http << std::endl;
        
        std::map<std::string, std::string>::const_iterator it = headers.begin();
        for(it; it != headers.end(); ++it)
        {
            std::cout << it->first << " " << it->second << std::endl;
        }
        
        it = params.begin();
        for(it; it != params.end(); ++it)
        {
            std::cout << it->first << " " << it->second << std::endl;
        }
    }
    
    std::string operator[](const std::string& key) const
    {
        std::map<std::string, std::string>::const_iterator it = params.find(key);
        if(it == params.end())
            return "";
        else
            return it->second;
    }
    
    template<typename T>
    T Header(const std::string& header)
    {
        long int num = strtol(headers[header].c_str(), 0, 0);
        return num;
    }
    
    template<>
    std::string Header(const std::string& header)
    {
        return headers[header];
    }
    
    std::map<std::string, std::string> Params() { return params; }
    std::string Method() const { return method; }
    std::string URI() const { return uri; }
    std::string Path() const { return path; }
private:
    void ParseParams(const std::string& str)
    {
        std::vector<std::string> p = util::split(str, "&");
        if(p.empty())
            p.push_back(str);
        
        for(unsigned i = 0; i < p.size(); ++i)
        {
            std::vector<std::string> kv = util::split(p[i], "=");
            if(kv.empty())
                params.insert(std::make_pair(p[i], ""));
            else
                params.insert(std::make_pair(kv[0], kv[1]));
        }
    }

    std::string method;
    std::string uri;
    std::string path;
    std::string http;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
};

#endif
