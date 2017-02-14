#include "httpserver.h"
#include "htmltemplate.h"
#include "database.h"

Database g_db;

DB_TABLE(
    ModuleData,
    (table_key) id,
    (std::string) name,
    (std::string) desc
);

DB_TABLE(
    ClassData,
    (table_key) id,
    (std::string) name,
    (std::string) desc
);

DB_TABLE(
    EncounterData,
    (table_key) id,
    (std::string) name,
    (std::string) desc,
    (int) exp
);

DB_TABLE(
    ItemData,
    (table_key) id,
    (std::string) name,
    (std::string) desc,
    (float) weight
);

DB_TABLE(
    LocationData,
    (table_key) id,
    (std::string) name,
    (std::string) desc,
    (int) population
);

DB_TABLE(
    SpellData,
    (table_key) id,
    (std::string) name,
    (std::string) desc,
    (int) level,
    (int) classId
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
    template<typename T>
    void FillJSON(nlohmann::json& json)
    {
        std::vector<T> result = 
            g_db.Select<T>({"id", "name"}).Exec();
        for(unsigned i = 0; i < result.size(); ++i)
        {
            nlohmann::json j = nlohmann::json::parse(result[i].ToJSON());
            json[T::Name()][i] = j;
        }
    }

    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("index.html");
        nlohmann::json json;
        FillJSON<CharacterData>(json);
        FillJSON<LocationData>(json);
        FillJSON<ItemData>(json);
        FillJSON<SpellData>(json);
        FillJSON<EncounterData>(json);
        FillJSON<ClassData>(json);
        
        std::vector<ModuleData> result = 
            g_db.Select<ModuleData>({"id", "name", "desc"}).Exec();
        nlohmann::json j = nlohmann::json::parse(result[0].ToJSON());
        json[ModuleData::Name()] = j;

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

class LocationHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("location.html");
        
        std::vector<LocationData> result =
            g_db.Select<LocationData>({}).Where("id").Eq(request["id"]).Exec();
            
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        tpl.SetJsonData(result[0].ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class ItemHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("item.html");
        
        std::vector<ItemData> result =
            g_db.Select<ItemData>({}).Where("id").Eq(request["id"]).Exec();
            
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        tpl.SetJsonData(result[0].ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class SpellHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("spell.html");
        
        std::vector<SpellData> result =
            g_db.Select<SpellData>({}).Where("id").Eq(request["id"]).Exec();
            
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        tpl.SetJsonData(result[0].ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class EncounterHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("encounter.html");
        
        std::vector<EncounterData> result =
            g_db.Select<EncounterData>({}).Where("id").Eq(request["id"]).Exec();
            
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        tpl.SetJsonData(result[0].ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class ClassHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        HTMLTemplate tpl("class.html");
        
        std::vector<ClassData> result =
            g_db.Select<ClassData>({}).Where("id").Eq(request["id"]).Exec();
            
        if(result.empty())
        {
            Handler404()(request, response);
            return;
        }
        tpl.SetJsonData(result[0].ToJSON());
        
        response.Header("Content-Type", "text/html; charset=UTF-8");
        response.Data(tpl.Render());
    }
};

class UpdateHandler : public HTTPRequestHandler
{
public:
    template<typename T>
    void Update(const HTTPRequest& request, HTTPResponse& response)
    { 
        g_db.Update<T>(
            {request["name"]},
            {util::urlDecode(request["value"])}
        ).Where("id").Eq(request["id"]).Exec();
    }
    
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        if(request["object"] == "character")  
            Update<CharacterData>(request, response);
        else if(request["object"] == "location")
            Update<LocationData>(request, response);
        else if(request["object"] == "item")
            Update<ItemData>(request, response);
        else if(request["object"] == "spell")
            Update<SpellData>(request, response);
        else if(request["object"] == "encounter")
            Update<EncounterData>(request, response);
        else if(request["object"] == "class")
            Update<ClassData>(request, response);
        else if(request["object"] == "module")
            Update<ModuleData>(request, response);
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
            charData.name = "New Character";
            charData.strength = 10;
            charData.dexterity = 10;
            charData.constitution = 10;
            charData.intelligence = 10;
            charData.wisdom = 10;
            charData.charisma = 10;
            g_db.Insert(charData).Exec();
        }
        else if(request["object"] == "location")
        {
            LocationData location = { 0 };
            location.name = "New Location";
            location.desc = "### Markdown Text";
            location.population = 0;
            g_db.Insert(location).Exec();
        }
        else if(request["object"] == "item")
        {
            ItemData item = { 0 };
            item.name = "New Item";
            item.desc = "Markdown description text";
            g_db.Insert(item).Exec();
        }
        else if(request["object"] == "spell")
        {
            SpellData data = { 0 };
            data.name = "New Spell";
            data.desc = "Markdown description text";
            data.level = 0;
            data.classId = 0;
            g_db.Insert(data).Exec();
        }
        else if(request["object"] == "encounter")
        {
            EncounterData data = { 0 };
            data.name = "New Encounter";
            data.desc = "Markdown description text";
            data.exp = 1;
            g_db.Insert(data).Exec();
        }
        else if(request["object"] == "class")
        {
            ClassData data = { 0 };
            data.name = "New Class";
            data.desc = "Markdown description text";
            g_db.Insert(data).Exec();
        }
        
        response.Status("302");
        response.Header("Location", "/");
    }
};

class UploadHandler : public HTTPRequestHandler
{
public:
    void operator()(const HTTPRequest& request, HTTPResponse& response)
    {
        std::vector<char> data = request.FileData("file");
        std::ofstream fout(
            std::string("data") + 
            "." + 
            util::FileNameExtension(request.FileName("file")), 
            std::ios::out | std::ios::binary
        );
        fout.write((char*)&data[0], data.size());
        fout.close();
    }
};

int main()
{
    g_db.Init();
    
    HTTPServer server;
    server.Init();
    
    server.SetHandler<IndexHandler>("/");
    server.SetHandler<CharacterHandler>("/character");
    server.SetHandler<LocationHandler>("/location");
    server.SetHandler<ItemHandler>("/item");
    server.SetHandler<SpellHandler>("/spell");
    server.SetHandler<EncounterHandler>("/encounter");
    server.SetHandler<ClassHandler>("/class");
    server.SetHandler<UpdateHandler>("/upd");
    server.SetHandler<AddHandler>("/add");
    server.SetHandler<UploadHandler>("/upload");
    
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
