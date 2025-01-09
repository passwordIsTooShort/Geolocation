#ifndef TESTS_MOCKS_NETWORKACCESSHANDLERMOCK_HPP_
#define TESTS_MOCKS_NETWORKACCESSHANDLERMOCK_HPP_

#include "intf/INetworkAccessHandler.hpp"

#include <gmock/gmock.h>

class NetworkAccessHandlerMock : public INetworkAccessHandler
{
 public:
  MOCK_METHOD(void, get, (const Url&, ResponseCallback&&), (override));
};

#endif // TESTS_MOCKS_NETWORKACCESSHANDLERMOCK_HPP_