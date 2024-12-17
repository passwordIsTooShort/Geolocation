#ifndef APP_SQLDATABASE_HPP_
#define APP_SQLDATABASE_HPP_

#include "IDatabase.hpp"

class SqlDatabase : public IDatabase
{
public:
    SqlDatabase() = default;
    virtual ~SqlDatabase() override = default;

    virtual bool connectToDatabase() override;

    virtual bool add(GeolocationData locationData, IpAdress address) override;

    virtual bool removeByIpAdress(IpAdress address) override;
    
    virtual bool removeByGeolocation(GeolocationData locationData) override;
    
    virtual GeolocationData getLocation(IpAdress address) const override;

private:
    std::string mPathToDatabase;
};
#endif // APP_SQLDATABASE_HPP_