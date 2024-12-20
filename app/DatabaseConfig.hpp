#ifndef APP_DATABASECONFIG_HPP_
#define APP_DATABASECONFIG_HPP_

#include <string>

class DatabaseConfig
{
public:
    enum class DatabaseType
    {
        SQL_LITE
    };

    DatabaseConfig(DatabaseType databaseType, std::string databasePath)
    : mType(databaseType)
    , mPath(databasePath)
    {}

    std::string getQtDatabaseType() const
    {
        switch (mType)
        {
            case DatabaseType::SQL_LITE:
                return "QSQLITE";
            default:
                return "";
        }
    }

    std::string getPath() const
    {
        return mPath;
    }

private:
    const DatabaseType mType;
    const std::string mPath;

};
#endif // APP_DATABASECONFIG_HPP_