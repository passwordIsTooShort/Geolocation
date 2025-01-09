#ifndef APP_ONLINELOCATIONPROVIDER_HPP_
#define APP_ONLINELOCATIONPROVIDER_HPP_

#include <optional>
#include <string>

#include "intf/ILocationProvider.hpp"
#include "intf/INetworkAccessHandler.hpp"
#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class OnlineLocationProvider : public ILocationProvider
{
public:
    OnlineLocationProvider(std::unique_ptr<INetworkAccessHandler> networkAccessHandler,
                           std::string hostName,
                           std::string accessKey);

    virtual ~OnlineLocationProvider() override = default;

    virtual void getByIp(IpAddress address,
                         SuccessCallback&& successCallback,
                         FailureCallback&& failureCallback) override;

private:
    inline static const std::string LATITUDE = "latitude";
    inline static const std::string LONGITUDE = "longitude";

    const std::string mHostName;
    const std::string mAccessKey;

    std::unique_ptr<INetworkAccessHandler> mNetworkAccessHandler;

    std::optional<GeolocationData> parseGeolocation(NetworkReply networkReply, std::string& failureInfo);
    bool checkNetworkReply(NetworkReply networkReply, std::string& failureInfo);
};
#endif // APP_ONLINELOCATIONPROVIDER_HPP_