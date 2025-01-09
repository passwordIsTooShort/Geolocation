#ifndef APP_NETWORKACCESSHANDLER_HPP_
#define APP_NETWORKACCESSHANDLER_HPP_

#include <QtNetwork/QNetworkAccessManager>

#include "intf/INetworkAccessHandler.hpp"

class NetworkAccessHandler : public INetworkAccessHandler
{
public:
    NetworkAccessHandler() = default;
    virtual ~NetworkAccessHandler() = default;

    virtual void get(const Url& request, ResponseCallback&& responseCallback);

private:
    QNetworkAccessManager mNetworkAccessManager;
};
#endif // APP_NETWORKACCESSHANDLER_HPP_