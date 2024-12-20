#ifndef APP_SQLITEDATABASE_HPP_
#define APP_SQLITEDATABASE_HPP_

#include "IDatabase.hpp"
#include "DatabaseConfig.hpp"
#include <QtCore/QString>
#include <QtSql/QSqlQuery>

class SQLiteDatabase : public IDatabase
{
public:
    SQLiteDatabase(DatabaseConfig config)
    : mDatabaseConfig(config)
    {}

    virtual ~SQLiteDatabase() override = default;

    virtual bool connectToDatabase() override;

    virtual bool prepareToUse() override;

    virtual bool add(GeolocationData locationData, IpAdress address) override;

    virtual bool removeByIpAdress(IpAdress address) override;
    
    virtual bool removeByGeolocation(GeolocationData locationData) override;
    
    virtual std::optional<GeolocationData> getLocation(IpAdress address) const override;

private:
    inline static const QString TABLE_NAME = "ip_locations";

    const DatabaseConfig mDatabaseConfig;

    bool executeQuery(QString query) const;
    bool executeQuery(QString query, QSqlQuery& queryObject) const;
};
#endif // APP_SQLITEDATABASE_HPP_