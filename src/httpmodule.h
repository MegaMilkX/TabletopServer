#ifndef HTTPMODULE_H
#define HTTPMODULE_H

class HTTPModule
{
public:
    virtual ~HTTPModule() {}
    virtual std::string HTML() = 0;
};

#endif
