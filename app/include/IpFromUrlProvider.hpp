#ifndef APP_IPFROMURLPROVIDER_HPP_
#define APP_IPFROMURLPROVIDER_HPP_

#include <functional>

#include "intf/IIpFromUrlProvider.hpp"
#include "IpAddress.hpp"

class IpFromUrlProvider : public IIpFromUrlProvider
{
public:
    IpFromUrlProvider() = default;

    virtual ~IpFromUrlProvider() = default;

    virtual void getIpForUrl(Url url, SuccessCallback successCallback, FailureCallback failureCallback) override;
};
#endif // APP_IPFROMURLPROVIDER_HPP_