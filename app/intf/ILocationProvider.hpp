#ifndef APP_ILOCATIONPROVIDER_HPP_
#define APP_ILOCATIONPROVIDER_HPP_

#include <functional>

#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class ILocationProvider
{
public:
    using SuccessCallback = std::function<void(GeolocationData geolocation)>;
    using FailureCallback = std::function<void(std::string error)>;

    ILocationProvider() = default;

    virtual ~ILocationProvider() = default;
    
    virtual void getByIp(IpAddress address,
                         SuccessCallback&& successCallback,
                         FailureCallback&& failureCallback) = 0;
};
#endif // APP_ILOCATIONPROVIDER_HPP_