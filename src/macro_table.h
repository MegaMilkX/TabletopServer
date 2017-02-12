#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "macro_for_each_arg.h"
#include "macro_typed_expr.h"
#include "macro_narg.h"

#define FIELD_DEF(INDEX, ARG) \
    TYPEOF(ARG) STRIP(ARG);
    
#define FIELD_CREATE_TABLE(INDEX, ARG) \
    fields.push_back(std::string(STRINGIFY(STRIP(ARG))) + " " + sql_type<TYPEOF(ARG)>());

#define FIELD_INDEX_NAME(INDEX, ARG) \
    if(GetFieldCount() - INDEX - 1 == index) return STRINGIFY(STRIP(ARG));
    
#define FIELD_NAME_INDEX(INDEX, ARG) \
    if(STRINGIFY(STRIP(ARG)) == field) return GetFieldCount() - INDEX - 1;

#define FIELD_NAME(INDEX, ARG) \
    fields.push_back(STRINGIFY(STRIP(ARG)));

#define FIELD_GET_ROW_VALUE(INDEX, ARG) \
    STRIP(ARG) = sqlite_get_row_value<TYPEOF(ARG)>(stmt, GetFieldIndex(STRINGIFY(STRIP(ARG))));
    
#define FIELD_PRINT(INDEX, ARG) \
    std::cout << STRINGIFY(STRIP(ARG)) << ": " << STRIP(ARG) << std::endl;
    
#define FIELD_TRY_GET_FROM_STMT(INDEX, ARG) \
    if(STRINGIFY(STRIP(ARG)) == fields[i]) \
    { \
        STRIP(ARG) = sqlite_get_row_value<TYPEOF(ARG)>(stmt, i); \
        continue; \
    }
    
#define FIELD_GET_JSON_VAL_BY_ID(INDEX, ARG) \
    if(GetFieldCount() - INDEX - 1 == index) return ToJsonVal(STRIP(ARG));

#define FIELD_GET_VALUE_BY_INDEX(INDEX, ARG) \
    if(GetFieldCount() - INDEX - 1 == index) return ToString(STRIP(ARG));
    
#define FIELD_GET_VALUE_BY_INDEX_INSERT(INDEX, ARG) \
    if(GetFieldCount() - INDEX - 1 == index) return ToStringSQLInsert(STRIP(ARG));
    
#define DB_TABLE(NAME, ...) \
    class NAME \
    { \
    public: \
        FOR_EACH_ARG(FIELD_DEF, __VA_ARGS__) \
        static unsigned GetFieldCount() { return PP_NARGS(__VA_ARGS__); } \
        static std::string GetFieldName(unsigned index) \
        { \
            FOR_EACH_ARG(FIELD_INDEX_NAME, __VA_ARGS__) \
            return ""; \
        } \
        static int GetFieldIndex(const std::string& field) \
        { \
            FOR_EACH_ARG(FIELD_NAME_INDEX, __VA_ARGS__) \
            return -1; \
        } \
        static std::vector<std::string> GetAllFieldNames() \
        { \
            std::vector<std::string> fields; \
            FOR_EACH_ARG(FIELD_NAME, __VA_ARGS__) \
            return fields; \
        } \
        static std::vector<std::string> GetAllFieldNamesWithSQLType() \
        { \
            std::vector<std::string> fields; \
            FOR_EACH_ARG(FIELD_CREATE_TABLE, __VA_ARGS__) \
            return fields; \
        } \
        void Print() \
        { \
            FOR_EACH_ARG(FIELD_PRINT, __VA_ARGS__) \
        } \
        static std::string Name() \
        { \
            return #NAME; \
        } \
        void GetValues(sqlite3_stmt* stmt, const std::vector<std::string> fields) \
        { \
            unsigned int colCount = sqlite3_data_count(stmt); \
            for(unsigned i = 0; i < colCount && i < fields.size(); ++i) \
            { \
                FOR_EACH_ARG(FIELD_TRY_GET_FROM_STMT, __VA_ARGS__) \
            } \
        } \
        std::string GetFieldValueForSQLInsert(int index) const \
        { \
            FOR_EACH_ARG(FIELD_GET_VALUE_BY_INDEX_INSERT, __VA_ARGS__); \
            return ""; \
        } \
        std::string GetFieldValue(int index) const \
        { \
            FOR_EACH_ARG(FIELD_GET_VALUE_BY_INDEX, __VA_ARGS__) \
            return ""; \
        } \
        std::string ToJSON() \
        { \
            std::string json = "{\n"; \
            for(unsigned i = 0; i < GetFieldCount(); ++i) \
            { \
                json += "\""; \
                json += GetFieldName(i); \
                json += "\": "; \
                json += GetJsonVal(i); \
                if(i < GetFieldCount() - 1) \
                    json += ",\n"; \
            } \
            json += "\n}\n"; \
            return json; \
        } \
        std::string GetJsonVal(unsigned index) \
        { \
            FOR_EACH_ARG(FIELD_GET_JSON_VAL_BY_ID, __VA_ARGS__) \
            return ""; \
        } \
    }

struct table_key
{
public:
  table_key() : _internal(0){}
  table_key(long long value) { _internal = value; }
  operator long long() const { return _internal; }
private:
  long long _internal;
};
    
template<typename T>
inline std::string ToJsonVal(const T& value)
{
    return std::to_string(value);
}

template<>
inline std::string ToJsonVal(const std::string& value)
{
    return std::string("\"") + value + "\"";
}

template<typename T>
inline std::string ToString(const T& value)
{
    return std::to_string(value);
}

template<>
inline std::string ToString(const std::string& value)
{
    return value;
}

template<typename T>
inline std::string ToStringSQLInsert(const T& value)
{
    return std::to_string(value);
}

template<>
inline std::string ToStringSQLInsert(const std::string& value)
{
    return std::string("'") + value + "'";
}

template<>
inline std::string ToStringSQLInsert(const table_key& value)
{
    return "NULL";
}
    
#endif
