#ifndef APP_BASICLOCATIONMANAGER_HPP_
#define APP_BASICLOCATIONMANAGER_HPP_

#include <optional>
#include <string>
#include <memory>
#include <map>
#include <set>

#include "ILocationManager.hpp"
#include "GeolocationData.hpp"
#include "IpAddress.hpp"
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

    virtual void addLocationOfIp(IpAddress ipAddress) override;

    virtual void addLocationOfUrl(std::string url) override;

    virtual void updateLocationOfIp(IpAddress ipAddress) override;

    virtual void updateLocationOfUrl(std::string url) override;

    virtual LocationStatus getIpLocationStatus(IpAddress ipAddress) override;

    virtual LocationStatus getUrlLocationStatus(std::string url) override;

    virtual std::optional<GeolocationData> getLocationOfIp(IpAddress ipAddress) override;

    virtual std::optional<GeolocationData> getLocationOfUrl(std::string url) override;

private:
    std::unique_ptr<IDatabase> mDatabase;
    std::unique_ptr<ILocationProvider> mLocationProvider;
    std::unique_ptr<IIpFromUrlProvider> mIpFromUrlProvider;

    std::map<IpAddress, std::string> mIpToUrlMap;

    std::set<IpAddress> mProcessingIps;
    std::set<IpAddress> mUrlsFailedToGetIp;
    std::set<IpAddress> mIpsFailedToGetLocation;

    bool isIpInProcesingList(IpAddress ip) const;
    void addFailedIp(IpAddress ip);
    bool isIpInFailedList(IpAddress ip) const;
    void addFailedUrl(std::string url);
    bool isUrlInFailedList(std::string url) const;

    void handleNewLocation(IpAddress ipAddress,
                           GeolocationData geolocationData);
    std::optional<std::string> getUrlForIp(IpAddress ipAddress) const;
};

#endif // APP_BASICLOCATIONMANAGER_HPP_