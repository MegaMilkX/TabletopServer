#ifndef HTMLTEMPLATE_H
#define HTMLTEMPLATE_H

#include <fstream>
#include <string>
#include <stack>

#include "ext/json.hpp"
#include "ext/luatemplater.h"

class HTMLTemplate
{
public:
    class Var
    {
    public:
        Var(size_t pos) : pos(pos), value("") {}
        Var& operator=(const std::string& val) { value = val; }
    private:
        size_t pos;
        std::string value;
    };
    
    HTMLTemplate(const std::string& path)
    {        
        std::ifstream file;
        std::string filePath = util::replace_char(path, '/', '\\');
        if(filePath[0] == '\\')
            filePath.erase(filePath.begin());
        file.open(filePath, std::ios::binary | std::ios::ate);
        if(file)
        {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(size);
            file.read(buffer.data(), size);
            
            data = std::string(buffer.begin(), buffer.end());
            
            //FindSlots(data);
        }
        file.close();
    }
    
    void SetJsonData(const std::string& jsonStr)
    {
        json = nlohmann::json::parse(jsonStr);
        std::string name;
        lua_State* L = tpl.GetState();
        if(json.is_object())
        {
            for(nlohmann::json::iterator it = json.begin(); it != json.end(); ++it)
            {
                SetJsonDataImpl(it.key(), *it, L);
                lua_setglobal(L, it.key().c_str());
            }
        }
    }
    
    void SetJsonDataImpl(std::string& name, nlohmann::json& json, lua_State* L)
    {
        if(json.is_null())
            return;
        else if(json.is_boolean())
        {
            lua_pushnumber(L, json.get<bool>());
            //tpl.set(name, json.get<bool>());
        }
        else if(json.is_number())
        {
            lua_pushnumber(L, json.get<int>());
            //tpl.set(name, json.get<int>());
        }
        else if(json.is_string())
        {
            lua_pushstring(L, json.get<std::string>().c_str());
            //tpl.set(name, json.get<std::string>());
        }
        else if(json.is_array())
        {
            lua_newtable(L);
            int i = 0;
            for(nlohmann::json::iterator it = json.begin(); it != json.end(); ++it)
            {
                lua_pushinteger(L, i+1);
                SetJsonDataImpl(std::to_string(i), *it, L);
                lua_settable(L, -3);
                i++;
            }
        }
        if(json.is_object())
        {
            lua_newtable(L);
            for(nlohmann::json::iterator it = json.begin(); it != json.end(); ++it)
            {
                lua_pushstring(L, (it.key()).c_str());
                SetJsonDataImpl(it.key(), *it, L);
                lua_settable(L, -3);
            }
        }
    }
    
    std::string Render()
    {
        return tpl.render(data);
    }
    
    template<typename T>
    HTMLTemplate& Place(const std::string& placeholder, const T& value)
    {
        return Place(placeholder, std::to_string(value));
    }
    
    template<>
    HTMLTemplate& Place(const std::string& placeholder, const std::string& value)
    {
        size_t pos;
        if(value.find("%%") != std::string::npos)
            return *this;
        
        std::string ph = "%%" + placeholder + "%%";
        while((pos = data.find(ph)) != std::string::npos)
            data.replace(pos, ph.size(), value);
        
        return *this;
    }
    
    operator std::string()
    {
        return data;
    }
private:
    std::string data;
    nlohmann::json json;
    LuaTemplater tpl;
};

#endif
