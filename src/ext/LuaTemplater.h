// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <vector>

struct lua_State;

#define STATIC static
#define VIRTUAL virtual

extern "C" {
    #include "ext/lua-5.1.5/src/lua.h"
    #include "ext/lua-5.1.5/src/lualib.h"
    #include "ext/lua-5.1.5/src/lauxlib.h"
}

class LuaTemplater {
private:
    lua_State *L;

    // [[[cog
    // import cog_addheaders
    // cog_addheaders.addv2()
    // ]]]
    // generated, using cog:

    public:
    LuaTemplater();
    ~LuaTemplater();
    void set(std::string name, std::string value);
    void set(std::string name, float value);
    void set(std::string name, int value);
    void set(std::string name, std::vector< std::string> &values);
    void set(std::string name, std::vector< int> &values);
    void set(std::string name, std::vector< float> &values);

    lua_State* GetState(){return L;}
    
    std::string render(std::string template_string);

    private:
    std::string getTemplaterSource();

    // [[[end]]]
};

