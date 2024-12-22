#ifndef APP_IPADDRESS_HPP_
#define APP_IPADDRESS_HPP_

#include <string>

class IpAddress
{
public:
    IpAddress(std::string ipAddress)
    : mIpAddress(ipAddress)
    {}
    virtual ~IpAddress() = default;

    std::string getIpAddress() const
    {
        return mIpAddress;
    }

    bool operator<(const IpAddress& other) const
    {
        return this->mIpAddress < other.mIpAddress;
    }

private:
    std::string mIpAddress;
};
#endif // APP_IPADDRESS_HPP_