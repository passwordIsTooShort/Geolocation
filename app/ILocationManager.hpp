#ifndef APP_ILOCATIONMANAGER_HPP_
#define APP_ILOCATIONMANAGER_HPP_

#include <optional>
#include <string>

#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class ILocationManager
{
public:
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

    virtual void addLocationOfIp(IpAddress ipAddress) = 0;

    virtual void addLocationOfUrl(std::string url) = 0;

    virtual void updateLocationOfIp(IpAddress ipAddress) = 0;

    virtual void updateLocationOfUrl(std::string url) = 0;

    virtual LocationStatus getIpLocationStatus(IpAddress ipAddress) = 0;

    virtual LocationStatus getUrlLocationStatus(std::string url) = 0;

    virtual std::optional<GeolocationData> getLocationOfIp(IpAddress ipAddress) = 0;

    virtual std::optional<GeolocationData> getLocationOfUrl(std::string url) = 0;
};

#endif // APP_ILOCATIONMANAGER_HPP_