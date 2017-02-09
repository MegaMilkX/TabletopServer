#ifndef HTTPMODULE_H
#define HTTPMODULE_H

#include <map>

class HTTPModule
{
public:
    virtual ~HTTPModule() {}
    virtual void Params(const std::map<std::string, std::string>& params) {}
    virtual std::string HTML()
    {
        std::string html;
        html += "<!DOCTYPE html>";
        html += "<html>";
        html += "<head>";
        html += "<meta charset=\"UTF-8\">";
        html += "<title>" + Title() + "</title>";
        html += Head();
        html += "</head>";
        html += "<body>" + Body() + "</body>";
        html += "</html>";
        return html;
    }
    virtual std::string Title() { return "Title"; }
    virtual std::string Head() { return ""; }
    virtual std::string Body() { return "Document body"; }
};

#endif
