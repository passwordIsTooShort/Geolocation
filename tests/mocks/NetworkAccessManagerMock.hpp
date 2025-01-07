#ifndef APP_NETWORKACCESSMANAGERMOCK_HPP_
#define APP_NETWORKACCESSMANAGERMOCK_HPP_

#include "intf/INetworkAccessManager.hpp"

#include "gmock/gmock.h"

class NetworkAccessManagerMock : public INetworkAccessManager
{
 public:
  MOCK_METHOD(QNetworkReply*, get, (QNetworkRequest), (override));
};

#endif // APP_NETWORKACCESSMANAGERMOCK_HPP_