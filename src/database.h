#ifndef DATABASE_H
#define DATABASE_H

#include "ext/sqlite3.h"
#pragma comment(lib, "../lib/sqlite3.lib")

#include "macro_table.h"

template<typename T>
class QuerySelect
{
public:
    QuerySelect(sqlite3* db, const std::string& tableName, const std::vector<std::string>& fields)
    {
        this->fields = fields;
        this->db = db;
        query += "SELECT";
        
        for(unsigned i = 0; i < fields.size(); ++i)
        {
            query += " ";
            query += fields[i];
            if(i < fields.size() - 1)
                query += ",";
        }
        
        if(fields.empty())
        {
            query += " * ";
            this->fields = T::GetAllFieldNames();
        }
        
        query += " FROM ";
        
        query += tableName;
        query += " ";
    }
    
    QuerySelect<T>& Where(const std::string& field)
    {
        query += "WHERE ";
        query += field;
        return *this;
    }
    
    QuerySelect<T>& Eq(const std::string& value)
    {
        query += " = ?";
        values.push_back(value);
        return *this;
    }
    
    std::vector<T> Exec()
    {
        std::cout << query << std::endl;
        std::vector<T> result;
        
        int rc = 0;
        sqlite3_stmt* stmt;
        
        if((rc = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
        {
            std::cout << "sqlite3_prepare_v2 failed" << std::endl;
            std::cout << sqlite3_errmsg(db) << std::endl;
            return result;
        }
        
        int loc = 0;
        for(unsigned i = 0; i < values.size(); ++i)
        {
            if((rc = sqlite3_bind_text(stmt, ++loc, values[i].c_str(), -1, SQLITE_TRANSIENT)) != SQLITE_OK)
            {
                std::cout << "sqlite3_bind_text failed" << std::endl;
                std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
            }
        }
        
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            T row = { 0 };
            row.GetValues(stmt, fields);
            result.push_back(row);
        }
        
        sqlite3_finalize(stmt);
        
        return result;
    }
private:
    std::string query;
    std::vector<std::string> fields;
    std::vector<std::string> values;
    sqlite3* db;
};

template<typename T>
class QueryUpdate
{
public:
    QueryUpdate(sqlite3* db,
                const std::string& tableName,
                const std::vector<std::string>& fields,
                const std::vector<std::string>& values)
                : db(db)
    {
        this->fields = fields;
        this->values = values;
        query = std::string("UPDATE ") + tableName + " SET";
        
        for(unsigned i = 0; i < fields.size() && i < values.size(); ++i)
        {
            query += std::string(" ");
            query += fields[i] + " = " + "?";
            if((i < fields.size() - 1) && (i < values.size() - 1))
                query += ",";
        }
        query += " ";
    }
    
    QueryUpdate<T>& Where(const std::string& field)
    {
        query += "WHERE ";
        query += field;
        return *this;
    }
    
    QueryUpdate<T>& Eq(const std::string& value)
    {
        query += " = ?";
        values.push_back(value);
        return *this;
    }
    
    void Exec()
    {
        std::cout << query << std::endl;
        
        int rc = 0;
        sqlite3_stmt* stmt;
        
        if((rc = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
        {
            std::cout << "sqlite3_prepare_v2 failed" << std::endl;
            std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        
        int loc = 0;
        for(unsigned i = 0; i < values.size(); ++i)
        {
            if((rc = sqlite3_bind_text(stmt, ++loc, values[i].c_str(), -1, SQLITE_TRANSIENT)) != SQLITE_OK)
            {
                std::cout << "sqlite3_bind_text failed" << std::endl;
                std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
            }
        }
        
        if((rc = sqlite3_step(stmt)) != SQLITE_DONE)
        {
            
        }
        
        sqlite3_finalize(stmt);
    }
private:
    std::string query;
    std::vector<std::string> fields;
    std::vector<std::string> values;
    sqlite3* db;
};

template<typename T>
class QueryInsert
{
public:
    QueryInsert(sqlite3* db,
                const std::string& tableName,
                const T& data)
                : db(db), data(data)
    {
        query = std::string("INSERT INTO ") + tableName + " (";
        
        for(unsigned i = 0; i < T::GetFieldCount(); ++i)
        {
            query += T::GetFieldName(i);
            if(i < T::GetFieldCount() - 1)
                query += ",";
        }
        query += ") ";
        
        query += "VALUES (";
        for(unsigned i = 0; i < T::GetFieldCount(); ++i)
        {
            query += "?";
            if(i < T::GetFieldCount() - 1)
                query += ",";
        }
        query += ")";
    }
    
    void Exec()
    {
        std::cout << query << std::endl;
        
        int rc = 0;
        sqlite3_stmt* stmt;
        
        if((rc = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
        {
            std::cout << "sqlite3_prepare_v2 failed" << std::endl;
            std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        
        int loc = 0;
        for(unsigned i = 0; i < T::GetFieldCount(); ++i)
        {
            std::string val = data.GetFieldValueForSQLInsert(i);
            if(val == "NULL")
            {
                if((rc = sqlite3_bind_null(stmt, ++loc)) != SQLITE_OK)
                {
                    std::cout << "sqlite3_bind_text failed" << std::endl;
                    std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
                }
                continue;
            }
            if((rc = sqlite3_bind_text(stmt, ++loc, val.c_str(), -1, SQLITE_TRANSIENT)) != SQLITE_OK)
            {
                std::cout << "sqlite3_bind_text failed" << std::endl;
                std::cout << rc << " " << sqlite3_errmsg(db) << std::endl;
            }
        }
        
        if((rc = sqlite3_step(stmt)) != SQLITE_DONE)
        {
            
        }
        
        sqlite3_finalize(stmt);
    }
private:
    std::string query;
    T data;
    sqlite3* db;
};

class Database
{
public:
    Database()
    : db(0) {}
    
    int Init()
    {
        char* errMsg = 0;
        int rc;
        
        rc = sqlite3_open("data.db", &db);
        
        if(rc)
        {
            return -1;
        }
        
        return 0;
    }
    
    template<typename T>
    void CreateTable()
    {
        CreateTableImpl(T::Name(), T::GetAllFieldNamesWithSQLType());
    }
    
    template<typename T>
    QuerySelect<T> Select(const std::vector<std::string>& fields = std::vector<std::string>())
    {
        CreateTable<T>();
        QuerySelect<T> query(db, T::Name(), fields);        
        return query;
    }
    
    template<typename T>
    QueryUpdate<T> Update(const std::vector<std::string>& fields, const std::vector<std::string>& values)
    {
        CreateTable<T>();
        QueryUpdate<T> query(db, T::Name(), fields, values);
        return query;
    }
    
    template<typename T>
    QueryInsert<T> Insert(const T& data)
    {
        CreateTable<T>();
        QueryInsert<T> query(db, T::Name(), data);
        return query;
    }
    
    void Cleanup()
    {
        if(db)
            sqlite3_close(db);
    }
    
    operator sqlite3*()
    {
        return db;
    }
private:
    void CreateTableImpl(const std::string& name, const std::vector<std::string>& fields)
    {
        if(tableExists[name] == true)
            return;
        
        std::string query = std::string("CREATE TABLE IF NOT EXISTS ") + name + " (";
        for(unsigned i = 0; i < fields.size(); ++i)
        {
            query += fields[i];
            if(i < fields.size() - 1)
                query += ", ";
        }
        
        query += ");";
        
        std::cout << query << std::endl;
        
        int result = 0;
        sqlite3_stmt* stmt;
        
        if((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
        {
            std::cout << "sqlite3_prepare_v2 failed" << std::endl;
            std::cout << sqlite3_errmsg(db) << std::endl;
            return;
        }
        
        if(sqlite3_step(stmt) != SQLITE_DONE)
        {
            std::cout << "sqlite3_step failed" << std::endl;
        }
        
        sqlite3_finalize(stmt);
        
        tableExists[name] = true;
    }
    
    std::map<std::string, bool> tableExists;
    sqlite3* db;
};

std::vector<std::string> operator,(const std::string& first, const std::string& second)
{
    std::vector<std::string> result;
    result.push_back(first);
    result.push_back(second);
    return result;
}

std::vector<std::string>& operator,(std::vector<std::string>& vec, const std::string& second)
{
    vec.push_back(second);
    return vec;
}
    
template<typename T>
std::string sql_type()
{ 
#pragma message ("sql_type() - unsupported type. Using BLOB") 
  return "BLOB";
}
template<>
inline std::string sql_type<table_key>(){ return "INTEGER PRIMARY KEY"; }
template<>
inline std::string sql_type<char>(){ return "INTEGER"; }
template<>
inline std::string sql_type<int>(){ return "INTEGER"; }
template<>
inline std::string sql_type<long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<long long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<unsigned char>(){ return "INTEGER"; }
template<>
inline std::string sql_type<unsigned int>(){ return "INTEGER"; }
//template<>
//inline std::string sql_type<unsigned long>(){ return "INTEGER"; }
//template<>
//inline std::string sql_type<unsigned long long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<float>(){ return "REAL"; }
template<>
inline std::string sql_type<double>(){ return "REAL"; }
template<>
inline std::string sql_type<std::string>(){ return "TEXT"; }

template<typename T>
T sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
#pragma message ("sqlite_get_row_value() - unsupported type. Reading as BLOB")
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return T;
    const void* blob = sqlite3_column_blob(stmt, col);
    T value;
    memcpy(&value, blob , sizeof(T));
    return value;
}
template<>
inline char                   sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return char();
    return sqlite3_column_int(stmt, col); 
}
template<>
inline int                    sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return int();
    return sqlite3_column_int(stmt, col);
}
template<>
inline long                   sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return long();
    return sqlite3_column_int64(stmt, col);
}
template<>
inline long long              sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return long long();
    return sqlite3_column_int64(stmt, col);
}
template<>
inline unsigned char          sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return unsigned char();
    return sqlite3_column_int(stmt, col);
}
template<>
inline unsigned int           sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return unsigned int();
    return sqlite3_column_int(stmt, col);
}
/*template<>
inline unsigned long          sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return unsigned long();
    return sqlite3_column_int(stmt, col);
}
template<>
inline unsigned long long     sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return unsigned long long();
    return sqlite3_column_int(stmt, col);
}*/
template<>
inline float                  sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return float();
    return sqlite3_column_double(stmt, col);
}
template<>
inline double                 sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return double();
    return sqlite3_column_double(stmt, col);
}
template<>
inline std::string            sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return std::string();
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col)));
}
template<>
inline table_key              sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
    if(sqlite3_column_type(stmt, col) == SQLITE_NULL)
        return table_key();
    return sqlite3_column_int64(stmt, col);
}

#endif
