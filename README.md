# HTTPServer

## How 2 use

```cpp
#include "httpserver.h"

class IndexHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data("hello world");
    }
};

int main()
{
    HTTPServer server;
    server.Init();
    
    server.SetHandler<IndexHandler>("/");
    
    server.SetMIME("css", "text/css");
    server.SetMIME("ico", "image/x-icon");
    server.SetMIME("png", "image/png");
    
    server.Run();
    
    server.Cleanup();
    return 0;
}
```
