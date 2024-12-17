#ifndef APP_IPADDRESS_HPP_
#define APP_IPADDRESS_HPP_

#include <string>

class IpAdress
{
public:
    IpAdress(std::string ipAddress)
    : mIpAddress(ipAddress)
    {}
    virtual ~IpAdress() = default;

    std::string getIpAddress() const
    {
        return mIpAddress;
    }

private:
    std::string mIpAddress;
};
#endif // APP_IPADDRESS_HPP_