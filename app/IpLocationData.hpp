#ifndef APP_IPLOCATIONDATA_HPP_
#define APP_IPLOCATIONDATA_HPP_

#include "IpAddress.hpp"
#include "Url.hpp"
#include "GeolocationData.hpp"

struct IpLocationData
{
    GeolocationData geolocationdata;
    IpAddress ipAddress;
    Url url = Url("");
};

#endif // APP_IPLOCATIONDATA_HPP_