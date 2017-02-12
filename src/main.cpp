#include "httpserver.h"
#include "htmltemplate.h"
#include "database.h"

Database g_db;

DB_TABLE(
    LocationData,
    (table_key) id,
    (std::string) name,
    (std::string) desc,
    (int) population
);

DB_TABLE(
    CharacterData,
    (table_key) id,
    (std::string) name,
    (int) classId,
    (int) level,
    (std::string) bio,
    (std::string) ideal,
    (std::string) bond,
    (std::string) flaw,
    (int) strength,
    (int) dexterity,
    (int) constitution,
    (int) intelligence,
    (int) wisdom,
    (int) charisma,
    (int) armorClass,
    (int) initiative,
    (int) speed    
);

class IndexHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("index.html");
        
        std::vector<CharacterData> result = 
            g_db.Select<CharacterData>({"id", "name"}).Exec();
        
        nlohmann::json json;
        for(unsigned i = 0; i < result.size(); ++i)
        {
            nlohmann::json j = nlohmann::json::parse(result[i].ToJSON());
            json["CharacterData"][i] = j;
        }
        std::cout << std::setw(4) << json << std::endl;
        tpl.SetJsonData(json.dump());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class Handler404 : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate html("404.html");
        response.Status("404");
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(html);
    }
};

class CharacterHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("character.html");
        
        std::vector<CharacterData> result = 
            g_db.Select<CharacterData>({}).Where("id").Eq(request["id"]).Exec();
        
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        CharacterData charData = result[0];
        
        /*
        html.Place("strMod", std::to_string((charData.strength - 10) / 2));
        html.Place("dexMod", std::to_string((charData.dexterity - 10) / 2));
        html.Place("conMod", std::to_string((charData.constitution - 10) / 2));
        html.Place("intMod", std::to_string((charData.intelligence - 10) / 2));
        html.Place("wisMod", std::to_string((charData.wisdom - 10) / 2));
        html.Place("chaMod", std::to_string((charData.charisma - 10) / 2));
        */
        
        tpl.SetJsonData(charData.ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class UpdateHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        request.Print();
        if(request["object"] == "character")
        {    
            g_db.Update<CharacterData>(
                {request["name"]},
                {util::urlDecode(request["value"])}
            ).Where("id").Eq(request["charId"]).Exec();
        }
    }
};

class AddHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        if(request["object"] == "character")
        {
            CharacterData charData = { 0 };
            charData.name = "Name";
            charData.strength = 10;
            charData.dexterity = 10;
            charData.constitution = 10;
            charData.intelligence = 10;
            charData.wisdom = 10;
            charData.charisma = 10;
            g_db.Insert(charData).Exec();
        }
        
        response.Status("302");
        response.Header("Location", "/");
    }
};

int main()
{
    g_db.Init();
    
    HTTPServer server;
    server.Init();
    
    server.SetHandler<IndexHandler>("/");
    server.SetHandler<CharacterHandler>("/character");
    server.SetHandler<UpdateHandler>("/upd");
    server.SetHandler<AddHandler>("/add");
    
    server.SetMIME("css", "text/css");
    server.SetMIME("ico", "image/x-icon");
    server.SetMIME("png", "image/png");
    server.SetMIME("html", "text/html");
    server.SetMIME("js", "application/javascript");
    server.SetMIME("jpg", "image/jpg");
    server.SetMIME("gif", "image/gif");
    server.SetMIME("woff2", "application/font-woff2");
    server.SetMIME("woff", "application/font-woff");
    server.SetMIME("ttf", "font/truetype");
    
    server.Run();
    
    server.Cleanup();
    
    g_db.Cleanup();
    return 0;
}