#ifndef APP_ONLINELOCATIONPROVIDER_HPP_
#define APP_ONLINELOCATIONPROVIDER_HPP_

#include <optional>
#include <QtNetwork/QNetworkAccessManager>

#include "ILocationProvider.hpp"
#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class OnlineLocationProvider : public ILocationProvider
{
public:
    OnlineLocationProvider(std::string hostName, std::string accessKey)
    : mHostName{hostName}
    , mAccessKey{accessKey}
    {}

    virtual ~OnlineLocationProvider() override = default;

    virtual bool prepareProvider() override;
    
    virtual void getByIp(IpAdress address);

private:
    inline constexpr static int OK_STATUS_CODE = 200;

    QNetworkAccessManager mNetworkAccessManager;
    
    const std::string mHostName;
    const std::string mAccessKey;

    static QString getIpFromRequest(QNetworkRequest);
};
#endif // APP_ONLINELOCATIONPROVIDER_HPP_