#ifndef APP_INTF_INETWORKACCESSHANDLER_HPP_
#define APP_INTF_INETWORKACCESSHANDLER_HPP_

#include <functional>

#include "Url.hpp"
#include "NetworkReply.hpp"

class INetworkAccessHandler
{
public:
    using ResponseCallback = std::function<void(NetworkReply)>;

    INetworkAccessHandler() = default;
    virtual ~INetworkAccessHandler() = default;

    virtual void get(const Url& request, ResponseCallback&& responseCallback) = 0;
};
#endif // APP_INTF_INETWORKACCESSHANDLER_HPP_