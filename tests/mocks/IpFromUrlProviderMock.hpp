#ifndef TESTS_MOCKS_IPFROMURLPROVIDERMOCK_HPP_
#define TESTS_MOCKS_IPFROMURLPROVIDERMOCK_HPP_

#include "intf/IIpFromUrlProvider.hpp"

#include <gmock/gmock.h>

class IpFromUrlProviderMock : public IIpFromUrlProvider
{
 public:
  MOCK_METHOD(void, getIpForUrl, (Url, SuccessCallback, FailureCallback), (override));
};

#endif // TESTS_MOCKS_IPFROMURLPROVIDERMOCK_HPP_