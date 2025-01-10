#ifndef TESTS_MOCKS_LOCATIONPROVIDERMOCK_HPP_
#define TESTS_MOCKS_LOCATIONPROVIDERMOCK_HPP_

#include "intf/ILocationProvider.hpp"

#include <gmock/gmock.h>

class LocationProviderMock : public ILocationProvider
{
 public:
  MOCK_METHOD(void, getByIp, (IpAddress, SuccessCallback&&, FailureCallback&&), (override));
};

#endif // TESTS_MOCKS_LOCATIONPROVIDERMOCK_HPP_