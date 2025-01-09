#include "IpFromUrlProvider.hpp"

#include <QtNetwork/QHostInfo>

void IpFromUrlProvider::getIpForUrl(Url url, SuccessCallback successCallback, FailureCallback failureCallback)
{
    QHostInfo::lookupHost(QString::fromStdString(url.toString()), [successCallback, failureCallback](const QHostInfo &host)
    {
        if (host.error() != QHostInfo::NoError)
        {
            failureCallback(host.errorString().toStdString());
            return;
        }

        const auto addresses = host.addresses();
        std::vector<IpAddress> result;
        result.reserve(addresses.size());

        std::transform(addresses.cbegin(), addresses.cend(), std::back_inserter(result), [](const auto& elem)
        {
            return elem.toString().toStdString();
        });

        successCallback(std::move(result));
    });
}
