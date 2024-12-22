#ifndef APP_IIPFROMURLPROVIDER_HPP_
#define APP_IIPFROMURLPROVIDER_HPP_

#include <functional>
#include <vector>
#include <string>

#include "IpAddress.hpp"

class IIpFromUrlProvider
{
public:
    using onSuccess = std::function<void(std::string url, std::vector<IpAddress>&& ip)>;
    using onFailure = std::function<void(std::string url, std::string failureInfo)>;

    IIpFromUrlProvider() = default;

    virtual ~IIpFromUrlProvider() = default;

    virtual void getIpForUrl(std::string url, onSuccess successCallback, onFailure failureCallback) = 0;
};
#endif // APP_IIPFROMURLPROVIDER_HPP_