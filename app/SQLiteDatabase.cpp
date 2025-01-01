#include "SQLiteDatabase.hpp"

#include <QtSql/QSqlDatabase>
#include <QtCore/QStringBuilder>
#include <QtCore/QLocale>

bool SQLiteDatabase::connectToDatabase()
{
    // TODO: In case of multiple database types - introduce json config file
    // that contains database type, database name, and optionally user name
    // and hostname (password need to be provided to the app).
    // Database can be created by factory based on config file
    QSqlDatabase database = QSqlDatabase::addDatabase(QString::fromStdString(mDatabaseConfig.getQtDatabaseType()));
    database.setDatabaseName(QString::fromStdString(mDatabaseConfig.getPath()));

    return database.isValid() && database.open();
}

bool SQLiteDatabase::prepareToUse()
{
    QString tableCreateQuery = "CREATE TABLE IF NOT EXISTS " %
                               TABLE_NAME %
                               " (latitude double not null," %
                               " longitude double not null," %
                               " ip text," %
                               " api_hash text);";

    return executeQuery(tableCreateQuery);
}

bool SQLiteDatabase::add(GeolocationData locationData, IpAddress address)
{
    return add(locationData, address, "");
}

bool SQLiteDatabase::add(GeolocationData locationData, IpAddress address, std::string url)
{
    // TODO: use QLocale instead QString::number to control precision
    // QLocale::FloatingPointShortest(20)

    QString addLocationQuery = "INSERT INTO " %
                               TABLE_NAME %
                               " (latitude," %
                               " longitude," %
                               " ip)" %
                               " VALUES" %
                               " ('" %
                               QString::number(locationData.getLatitude()) %
                               "', '" %
                               QString::number(locationData.geLongitude()) %
                               "', '" %
                               QString::fromStdString(address.toString()) %
                               "')";

    return executeQuery(addLocationQuery);
}

bool SQLiteDatabase::removeByIpAddress(IpAddress address)
{
    QString removeLocationQuery = "DELETE FROM " %
                                  TABLE_NAME %
                                  " WHERE" %
                                  " (ip = '" %
                                  QString::fromStdString(address.toString()) %
                                  "');";
    return executeQuery(removeLocationQuery);
}

bool SQLiteDatabase::removeByGeolocation(GeolocationData locationData)
{
    QString removeLocationQuery = "DELETE FROM " %
                                  TABLE_NAME %
                                  " WHERE" %
                                  " (latitude = '" %
                                  QString::number(locationData.getLatitude()) %
                                  "' AND" %
                                  " longitude = '" %
                                  QString::number(locationData.geLongitude()) %
                                  "');";
    return executeQuery(removeLocationQuery);
}

std::optional<GeolocationData> SQLiteDatabase::getLocation(IpAddress address) const
{
    QString selectIpQuery = "SELECT latitude, longitude FROM " %
                            TABLE_NAME %
                            " WHERE" %
                            " (ip = '" %
                            QString::fromStdString(address.toString()) %
                            "');";

    QSqlQuery sqlQuery;
    const auto queryResult = executeQuery(selectIpQuery, sqlQuery);
    if (!queryResult)
    {
        qWarning() << "Failed to select geolocation from database - sql error";
        return std::nullopt;
    }

    int results = 0;    
    constexpr int LATITUDE_ID = 0;
    constexpr int LONGITUDE_ID = 1;
    double retrievedLatitude = -1;
    double retrievedLongitude = -1;
    while(sqlQuery.next())
    {
        ++results;
        retrievedLatitude = sqlQuery.value(LATITUDE_ID).toDouble();
        retrievedLongitude = sqlQuery.value(LONGITUDE_ID).toDouble();
    }

    if (results == 0)
    {
        return std::nullopt;
    }

    if (results > 1)
    {
        qWarning() << "There are more than 1 locations stored for ip: "
                   << QString::fromStdString(address.toString())
                   << ". Retrieved locations: " << results
                   << ". Returning last location from query.";
    }

    return std::optional<GeolocationData>(GeolocationData(retrievedLatitude, retrievedLongitude));
}

bool SQLiteDatabase::executeQuery(QString query) const
{
    QSqlQuery queryObject;
    return executeQuery(query, queryObject);
}

bool SQLiteDatabase::executeQuery(QString query, QSqlQuery& queryObject) const
{
    const auto queryPreparationResult = queryObject.prepare(query);
    if (!queryPreparationResult)
    {
        qWarning() << "Error while preparing query: \n" << query << "\n. SQL query syntax error\n";
        return false;
    }

    auto queryExecutionResult = queryObject.exec();
    if (!queryExecutionResult)
    {
        qWarning() << "Error while executing query: \n" << query << "\n";
        return false;
    }

    return true;
}