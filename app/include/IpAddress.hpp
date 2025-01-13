#ifndef APP_IPADDRESS_HPP_
#define APP_IPADDRESS_HPP_

#include <string>
#include <arpa/inet.h>

class IpAddress
{
public:
    IpAddress(std::string ipAddress)
    : mIpAddress(ipAddress)
    {}
    virtual ~IpAddress() = default;

    std::string toString() const
    {
        return mIpAddress;
    }

    bool operator<(const IpAddress& other) const
    {
        return this->mIpAddress < other.mIpAddress;
    }

    static bool isIpAddress(std::string input)
    {
        struct sockaddr_in sa;
        return (inet_pton(AF_INET, input.c_str(), &(sa.sin_addr)) != 0) ||
               (inet_pton(AF_INET6, input.c_str(), &(sa.sin_addr)) != 0);
    }

private:
    std::string mIpAddress;
};
#endif // APP_IPADDRESS_HPP_