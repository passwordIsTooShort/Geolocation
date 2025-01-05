#ifndef APP_ONLINELOCATIONPROVIDER_HPP_
#define APP_ONLINELOCATIONPROVIDER_HPP_

#include <optional>
#include <QtNetwork/QNetworkAccessManager>

#include "intf/ILocationProvider.hpp"
#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class OnlineLocationProvider : public ILocationProvider
{
public:
    OnlineLocationProvider(std::string hostName, std::string accessKey);

    virtual ~OnlineLocationProvider() override = default;

    virtual void getByIp(IpAddress address,
                         SuccessCallback&& successCallback,
                         FailureCallback&& failureCallback) override;

private:
    inline constexpr static int OK_STATUS_CODE = 200;

    // TODO: Provide it in constructor to allow mocking (and create factory for OnlineLocationProvider)
    QNetworkAccessManager mNetworkAccessManager;

    const QString mHostName;
    const QString mAccessKey;

    std::optional<GeolocationData> parseGeolocationFromReply(QNetworkReply* reply, std::string& failureInfo);
    bool checkNetworkReply(QNetworkReply* reply, std::string& failureInfo);
};
#endif // APP_ONLINELOCATIONPROVIDER_HPP_