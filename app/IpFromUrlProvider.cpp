#include "IpFromUrlProvider.hpp"

#include <QtNetwork/QHostInfo>

void IpFromUrlProvider::getIpForUrl(std::string url, onSuccess successCallback, onFailure failureCallback)
{
    QHostInfo::lookupHost(QString::fromStdString(url), [url, successCallback, failureCallback](const QHostInfo &host)
    {
        if (host.error() != QHostInfo::NoError)
        {
            failureCallback(url, host.errorString().toStdString());
            return;
        }

        const auto addresses = host.addresses();
        std::vector<IpAddress> result;
        result.reserve(addresses.size());

        std::transform(addresses.cbegin(), addresses.cend(), std::back_inserter(result), [](const auto& elem)
        {
            return elem.toString().toStdString();
        });

        successCallback(url, std::move(result));
    });
}
