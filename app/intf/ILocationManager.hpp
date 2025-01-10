#ifndef APP_INTF_ILOCATIONMANAGER_HPP_
#define APP_INTF_ILOCATIONMANAGER_HPP_

#include <optional>
#include <string>
#include <functional>

#include "IpLocationData.hpp"
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
        NOT_EXIST,
        WRONG_STATUS
    };

    enum class ManagerStatus
    {
        READY_TO_USE,
        DATABASE_ERROR,
        NOT_INITIALIZED
    };

    ILocationManager() = default;

    virtual ~ILocationManager() = default;

    virtual ManagerStatus getStatus() const = 0;

    virtual void addLocation(std::string ipOrUrl) = 0;

    virtual void updateLocation(std::string ipOrUrl) = 0;

    virtual bool removeLocation(std::string ipOrUrl) = 0;

    virtual LocationStatus getLocationStatus(std::string ipOrUrl) = 0;

    virtual std::vector<IpLocationData> getLocations(std::string ipOrUrl) = 0;
};

#endif // APP_INTF_ILOCATIONMANAGER_HPP_