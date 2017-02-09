#include "httpserver.h"

class IndexPage : public HTTPModule
{
public:
    void Params(const std::map<std::string, std::string>& params)
    {
        this->params = params;
    }

    std::string Title()
    {
        return "D&D Thing";
    }
    
    std::string Head()
    {
        std::string head;
        
        head += "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">";
        
        return head;
    }
    
    std::string Body()
    {
        std::string body;
        body += "<h1>D&D Tool</h1>";
        
        body += "<table border=\"1\">";
        body += "<tr class=\"table-title\">";
        
        std::map<std::string, std::string>::const_iterator it = params.begin();
        for(it; it != params.end(); ++it)
        {
            body += "<td>";
            body += it->first;
            body += "</td>";
        }
        
        body += "</tr>";
        body += "<tr>";
        
        it = params.begin();
        for(it; it != params.end(); ++it)
        {
            body += "<td>";
            body += it->second;
            body += "</td>";
        }
        
        body += "</tr>";
        body += "</table>";
        
        return body;
    }
    
private:
    std::map<std::string, std::string> params;
};

int main()
{
    HTTPServer server;
    server.Init();
    server.AddModule<IndexPage>("/");
    //server.AddModule<FileGet>("GET");
    
    server.AddMIME("css", "text/css");
    server.AddMIME("ico", "image/x-icon");
    server.AddMIME("png", "image/png");
    
    server.Run();
    
    server.Cleanup();
    return 0;
}