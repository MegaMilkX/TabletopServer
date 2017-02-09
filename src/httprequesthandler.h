#ifndef HTTPREQUESTHANDLER_H
#define HTTPREQUESTHANDLER_H

#include "httprequest.h"
#include "httpresponse.h"

class HTTPRequestHandler
{
public:
    virtual ~HTTPRequestHandler() {}
    virtual void operator()(const HTTPRequest& request, HTTPResponse& response) = 0;
};

#endif
