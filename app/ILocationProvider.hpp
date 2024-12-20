#ifndef APP_ILOCATIONPROVIDER_HPP_
#define APP_ILOCATIONPROVIDER_HPP_

#include "GeolocationData.hpp"
#include "IpAddress.hpp"

class ILocationProvider
{
public:
    using GeolocationCallback = std::function<void(IpAdress ip, std::string apiKey, GeolocationData geolocation)>;

    ILocationProvider() = default;

    virtual ~ILocationProvider() = default;
    
    virtual void getByIp(IpAdress address) = 0;

    virtual void setOnNewGeolocationCallback(GeolocationCallback&& callback)
    {
        mOnNewGeolocationCallback = callback;
    }

protected:
    GeolocationCallback mOnNewGeolocationCallback;
};
#endif // APP_ILOCATIONPROVIDER_HPP_