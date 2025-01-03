#ifndef APP_ILOCATIONMANAGER_HPP_
#define APP_ILOCATIONMANAGER_HPP_

#include <optional>
#include <string>
#include <functional>

#include "IpLocationData.hpp"
#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class ILocationManager
{
public:
    using NewLocationCallback = std::function<void(IpLocationData ipLocationData)>;

    enum class LocationStatus
    {
        READY_TO_READ,
        IN_PROGRESS,
        FAILED_TO_GET_LOCATION,
        FAILED_TO_GET_IP,
        NOT_EXIST
    };

    ILocationManager() = default;

    virtual ~ILocationManager() = default;

    virtual void addLocation(IpAddress ipAddress) = 0;

    virtual void addLocation(Url url) = 0;

    virtual void updateLocation(IpAddress ipAddress) = 0;

    virtual void updateLocation(Url url) = 0;

    virtual LocationStatus getLocationStatus(IpAddress ipAddress) = 0;

    virtual LocationStatus getLocationStatus(Url url) = 0;

    virtual std::vector<IpLocationData> getLocations(IpAddress ipAddress) = 0;

    virtual std::vector<IpLocationData> getLocations(Url url) = 0;

    virtual void setOnNewLocationCallback(NewLocationCallback&& newLocationCallback)
    {
        mNewLocationCallback = std::move(newLocationCallback);
    }

protected:
    NewLocationCallback mNewLocationCallback;
};

#endif // APP_ILOCATIONMANAGER_HPP_