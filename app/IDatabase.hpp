#ifndef APP_IDATABASE_HPP_
#define APP_IDATABASE_HPP_

#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class IDatabase
{
public:
    IDatabase() = default;
    virtual ~IDatabase() = default;

    virtual bool connectToDatabase() = 0;

    virtual bool add(GeolocationData locationData, IpAdress address) = 0;

    virtual bool removeByIpAdress(IpAdress address) = 0;
    
    virtual bool removeByGeolocation(GeolocationData locationData) = 0;
    
    virtual GeolocationData getLocation(IpAdress address) const = 0;
};
#endif // APP_IDATABASE_HPP_