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

    virtual bool add(IpLocationData ipLocationData) override;

    virtual bool remove(IpAddress address) override;

    virtual bool remove(Url url) override;

    virtual bool remove(GeolocationData locationData) override;

    virtual std::vector<IpLocationData> getLocations(IpAddress address) const override;

    virtual std::vector<IpLocationData> getLocations(Url url) const override;

private:
    /** @brief: Precision of IP stack is "often within a few meters
     * of the actual location" (based on ipstack description
     * ipstack.com/address-to-geolocation-api). Therefore, based on
     * decimal degrees notation precision, it is more than enough to
     * store loation with precision of 5 decimal places.
     * It gives accuracy of +- 1.11 m
     */
    inline static constexpr int DECIMAL_DEGREES_SAVE_PRECISION = 5;

    /** @brief Configuration used to create database */
    const DatabaseConfig mDatabaseConfig;

    /** @brief Method returns list of database table entries that
     * matches query provided as parameter
     *
     * @param queryString Query that will be executed on database to get
     * list of matching locations, ips and urls.
     *
     * @return List of matching results for given query will be returned.
     * If there will be no results, or query will fail, then empty vector
     * will be returned.
     */
    std::vector<IpLocationData> getTableEntries(QString queryString) const;

    /** @brief Helper method to execute sql query.
     *
     * This version is designed to use, when sql query
     * does not return anything meaningfull. Method returns
     * only sql execution result.
     *
     * @param query Query that will to be executed
     * @return True, if execution was processed without
     * any errors returned from database. False otherwise.
     */

    bool executeQuery(QString query) const;

    /** @brief Helper method to execute sql query.
     *
     * This version is designed to use, when sql query
     * returns something that need to be captured (e.g.
     * SELECT command). In this case, QSqlQuery object need
     * to be provided to the method. It allows, to get result
     * from this object after query execution.
     *
     * Remember, that reading query result is possible only
     * if method returned true
     *
     * @param query Query that will to be executed
     *
     * @return True, if execution was processed without
     * any errors returned from database. False otherwise.
     */
    bool executeQuery(QString query, QSqlQuery& queryObject) const;
};
#endif // APP_SQLITEDATABASE_HPP_