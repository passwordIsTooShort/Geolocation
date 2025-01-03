#ifndef APP_BASICLOCATIONMANAGER_HPP_
#define APP_BASICLOCATIONMANAGER_HPP_

#include <string>
#include <memory>
#include <map>
#include <set>
#include <variant>

#include "ILocationManager.hpp"
#include "IpLocationData.hpp"
#include "IpAddress.hpp"
#include "Url.hpp"
#include "IDatabase.hpp"
#include "ILocationProvider.hpp"
#include "IIpFromUrlProvider.hpp"

class BasicLocationManager : public ILocationManager
{
public:
    BasicLocationManager(std::unique_ptr<IDatabase> database,
                         std::unique_ptr<ILocationProvider> locationProvider,
                         std::unique_ptr<IIpFromUrlProvider> ipFromUrlProvider);

    virtual ~BasicLocationManager() = default;

    virtual void addLocation(IpAddress ipAddress) override;

    virtual void addLocation(Url url) override;

    virtual void updateLocation(IpAddress ipAddress) override;

    virtual void updateLocation(Url url) override;

    virtual LocationStatus getLocationStatus(IpAddress ipAddress) override;

    virtual LocationStatus getLocationStatus(Url url) override;

    virtual std::vector<IpLocationData> getLocations(IpAddress ipAddress) override;

    virtual std::vector<IpLocationData> getLocations(Url url) override;

private:
    std::unique_ptr<IDatabase> mDatabase;
    std::unique_ptr<ILocationProvider> mLocationProvider;
    std::unique_ptr<IIpFromUrlProvider> mIpFromUrlProvider;

    std::map<IpAddress, Url> mIpToUrlMap;

    std::set<std::variant<IpAddress, Url>> mProcessingElements;
    std::set<std::variant<IpAddress, Url>> mFailedElements;

    template<typename T>
    void insideAddLocation(T addressOrUrl)
    {
        // TODO: static assert or other verification, that I is IpAddress or Url
        const auto locationStatus = getLocationStatus(addressOrUrl);
        if (locationStatus == LocationStatus::READY_TO_READ)
        {
            std::cout << "Element: " << addressOrUrl.toString() << " already exists in database."
                    << " To force update it, run: method updateLocation.\n";
            return;
        }
        else if(locationStatus == LocationStatus::IN_PROGRESS)
        {
            std::cout << "Element: " << addressOrUrl.toString() << " is currently processing."
                    << " Wait to finish this request.\n";
            return;
        }

        updateLocation(addressOrUrl);
    }

    void startProcessing(std::variant<IpAddress, Url> ipOrUrl);
    void finishedSuccesfully(std::variant<IpAddress, Url> ipOrUrl);
    void finishedWithFail(std::variant<IpAddress, Url> ipOrUrl);
    bool isInProcesingList(std::variant<IpAddress, Url> ipOrUrl) const;
    bool isMarkedAsFailed(std::variant<IpAddress, Url> ipOrUrl) const;

    void handleNewLocation(IpAddress ipAddress,
                           GeolocationData geolocationData);
};

#endif // APP_BASICLOCATIONMANAGER_HPP_