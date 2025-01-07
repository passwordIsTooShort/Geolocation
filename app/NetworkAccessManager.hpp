#ifndef APP_NETWORKACCESSMANAGER_HPP_
#define APP_NETWORKACCESSMANAGER_HPP_

#include "intf/INetworkAccessManager.hpp"

class NetworkAccessManager : public INetworkAccessManager
{
public:
    NetworkAccessManager() = default;
    virtual ~NetworkAccessManager() = default;

    virtual QNetworkReply* get(QNetworkRequest request) override
    {
        return mNetworkAccessManager.get(request);
    }

private:
    QNetworkAccessManager mNetworkAccessManager;
};
#endif // APP_NETWORKACCESSMANAGER_HPP_