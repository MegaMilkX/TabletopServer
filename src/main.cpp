#include "httpserver.h"

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