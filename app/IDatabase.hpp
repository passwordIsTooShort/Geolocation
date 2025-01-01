#ifndef APP_IDATABASE_HPP_
#define APP_IDATABASE_HPP_

#include <optional>

#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class IDatabase
{
public:
    IDatabase() = default;
    virtual ~IDatabase() = default;

    virtual bool connectToDatabase() = 0;

    virtual bool prepareToUse() = 0;

    virtual bool add(GeolocationData locationData, IpAddress address) = 0;

    virtual bool add(GeolocationData locationData, IpAddress address, std::string url) = 0;

    virtual bool removeByIpAddress(IpAddress address) = 0;
    
    virtual bool removeByGeolocation(GeolocationData locationData) = 0;
    
    virtual std::optional<GeolocationData> getLocation(IpAddress address) const = 0;
};
#endif // APP_IDATABASE_HPP_