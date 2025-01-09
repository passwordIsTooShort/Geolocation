#ifndef APP_INTF_IDATABASE_HPP_
#define APP_INTF_IDATABASE_HPP_

#include <vector>

#include "IpAddress.hpp"
#include "Url.hpp"
#include "IpLocationData.hpp"

class IDatabase
{
public:
    IDatabase() = default;
    virtual ~IDatabase() = default;

    virtual bool connectToDatabase() = 0;

    virtual bool prepareToUse() = 0;

    virtual bool add(IpLocationData ipLocationData) = 0;

    virtual bool remove(IpAddress address) = 0;

    virtual bool remove(Url url) = 0;

    virtual bool remove(GeolocationData locationData) = 0;

    virtual std::vector<IpLocationData> getLocations(IpAddress address) const = 0;

    virtual std::vector<IpLocationData> getLocations(Url url) const = 0;
};
#endif // APP_INTF_IDATABASE_HPP_