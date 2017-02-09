#ifndef HTTPUTIL_H
#define HTTPUTIL_H

#include <string>
#include <vector>

inline std::vector<std::string> split(const std::string& str, const std::string& delim)
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

inline std::string replace_char(const std::string& str, char from, char to)
{
    std::string result = str;
    for(unsigned i = 0; i < result.size(); ++i)
    {
        if(result[i] == from)
            result[i] = to;
    }
    return result;
}

inline std::string FileNameExtension(const std::string& fileName)
{
    size_t pos = fileName.find_last_of('.');
    return fileName.substr(pos + 1, std::string::npos);
}

#endif
