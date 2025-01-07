#ifndef APP_INETWORKACCESSMANAGER_HPP_
#define APP_INETWORKACCESSMANAGER_HPP_

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class INetworkAccessManager
{
public:
    INetworkAccessManager() = default;
    virtual ~INetworkAccessManager() = default;

    virtual QNetworkReply* get(QNetworkRequest request) = 0;
};
#endif // APP_INETWORKACCESSMANAGER_HPP_