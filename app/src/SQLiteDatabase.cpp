#include "SQLiteDatabase.hpp"

#include <QtSql/QSqlDatabase>
#include <QtCore/QStringBuilder>
#include <QtCore/QLocale>

bool SQLiteDatabase::connectToDatabase()
{
    QSqlDatabase database = QSqlDatabase::addDatabase(QString::fromStdString(mDatabaseConfig.getQtDatabaseType()));
    database.setDatabaseName(QString::fromStdString(mDatabaseConfig.getPath()));

    return database.isValid() && database.open();
}

bool SQLiteDatabase::prepareToUse()
{
    QString tableCreateQuery = "CREATE TABLE IF NOT EXISTS " %
                               QString::fromStdString(mDatabaseConfig.getTableName()) %
                               " (latitude double not null," %
                               " longitude double not null," %
                               " ip text UNIQUE," %
                               " url text);";

    return executeQuery(tableCreateQuery);
}

bool SQLiteDatabase::add(IpLocationData ipLocationData)
{
    QString addLocationQuery = "INSERT OR REPLACE INTO " %
                               QString::fromStdString(mDatabaseConfig.getTableName()) %
                               " (latitude," %
                               " longitude," %
                               " ip," %
                               " url)" %
                               " VALUES" %
                               " ('" %
                               QString::number(ipLocationData.geolocationdata.latitude,
                                               'f', DECIMAL_DEGREES_SAVE_PRECISION) %
                               "', '" %
                               QString::number(ipLocationData.geolocationdata.longitude,
                                               'f', DECIMAL_DEGREES_SAVE_PRECISION) %
                               "', '" %
                               QString::fromStdString(ipLocationData.ipAddress.toString()) %
                               "', '" %
                               QString::fromStdString(ipLocationData.url.toString()) %
                               "')";

    return executeQuery(addLocationQuery);
}

bool SQLiteDatabase::remove(IpAddress address)
{
    QString removeLocationQuery = "DELETE FROM " %
                                  QString::fromStdString(mDatabaseConfig.getTableName()) %
                                  " WHERE" %
                                  " (ip = '" %
                                  QString::fromStdString(address.toString()) %
                                  "');";
    return executeQuery(removeLocationQuery);
}

bool SQLiteDatabase::remove(Url url)
{
    QString removeLocationQuery = "DELETE FROM " %
                                  QString::fromStdString(mDatabaseConfig.getTableName()) %
                                  " WHERE" %
                                  " (url = '" %
                                  QString::fromStdString(url.toString()) %
                                  "');";
    return executeQuery(removeLocationQuery);
}

bool SQLiteDatabase::remove(GeolocationData locationData)
{
    QString removeLocationQuery = "DELETE FROM " %
                                  QString::fromStdString(mDatabaseConfig.getTableName()) %
                                  " WHERE" %
                                  " (latitude = '" %
                                  QString::number(locationData.latitude,
                                                  'f', DECIMAL_DEGREES_SAVE_PRECISION) %
                                  "' AND" %
                                  " longitude = '" %
                                  QString::number(locationData.longitude,
                                                  'f', DECIMAL_DEGREES_SAVE_PRECISION) %
                                  "');";
    return executeQuery(removeLocationQuery);
}

std::vector<IpLocationData> SQLiteDatabase::getLocations(IpAddress address) const
{
    QString selectIpQuery = "SELECT latitude, longitude, ip, url FROM " %
                            QString::fromStdString(mDatabaseConfig.getTableName()) %
                            " WHERE" %
                            " (ip = '" %
                            QString::fromStdString(address.toString()) %
                            "');";
    const auto results = getTableEntries(selectIpQuery);
    if (results.size() > 1)
    {
        qWarning() << "There are more than 1 locations stored for ip: "
                   << QString::fromStdString(address.toString())
                   << ". Number of retrieved locations: " << results.size();
    }
    return results;
}

std::vector<IpLocationData> SQLiteDatabase::getLocations(Url url) const
{
    QString selectIpQuery = "SELECT latitude, longitude, ip, url FROM " %
                            QString::fromStdString(mDatabaseConfig.getTableName()) %
                            " WHERE" %
                            " (url = '" %
                            QString::fromStdString(url.toString()) %
                            "');";
    return getTableEntries(selectIpQuery);
}

std::vector<IpLocationData> SQLiteDatabase::getTableEntries(QString queryString) const
{
    QSqlQuery sqlQuery;
    const auto queryResult = executeQuery(queryString, sqlQuery);
    if (!queryResult)
    {
        qWarning() << "Failed to select geolocation from database - sql error";
        return {};
    }

    constexpr int LATITUDE_ID = 0;
    constexpr int LONGITUDE_ID = 1;
    constexpr int IP_ID = 2;
    constexpr int URL_ID = 3;

    std::vector<IpLocationData> databaseResults;
    while(sqlQuery.next())
    {
        double latitiude = sqlQuery.value(LATITUDE_ID).toDouble();
        double longtitiude = sqlQuery.value(LONGITUDE_ID).toDouble();
        IpAddress ip(sqlQuery.value(IP_ID).toString().toStdString());
        Url url(sqlQuery.value(URL_ID).toString().toStdString());

        databaseResults.emplace_back(IpLocationData{latitiude, longtitiude, ip, url});
    }

    return databaseResults;
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
