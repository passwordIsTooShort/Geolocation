#ifndef APP_IIPFROMURLPROVIDER_HPP_
#define APP_IIPFROMURLPROVIDER_HPP_

#include <functional>
#include <vector>
#include <string>

#include "IpAddress.hpp"
#include "Url.hpp"

class IIpFromUrlProvider
{
public:
    using SuccessCallback = std::function<void(std::vector<IpAddress>&& ip)>;
    using FailureCallback = std::function<void(std::string failureInfo)>;

    IIpFromUrlProvider() = default;

    virtual ~IIpFromUrlProvider() = default;

    virtual void getIpForUrl(Url url, SuccessCallback successCallback, FailureCallback failureCallback) = 0;
};
#endif // APP_IIPFROMURLPROVIDER_HPP_