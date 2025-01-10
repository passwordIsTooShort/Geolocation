#ifndef TESTS_MOCKS_DATABASEMOCK_HPP_
#define TESTS_MOCKS_DATABASEMOCK_HPP_

#include "intf/IDatabase.hpp"

#include <gmock/gmock.h>

class DatabaseMock : public IDatabase
{
 public:
  MOCK_METHOD(bool, connectToDatabase, (), (override));
  MOCK_METHOD(bool, prepareToUse, (), (override));
  MOCK_METHOD(bool, add, (IpLocationData), (override));
  MOCK_METHOD(bool, remove, (IpAddress), (override));
  MOCK_METHOD(bool, remove, (Url), (override));
  MOCK_METHOD(bool, remove, (GeolocationData), (override));
  MOCK_METHOD(std::vector<IpLocationData>, getLocations, (IpAddress), (const override));
  MOCK_METHOD(std::vector<IpLocationData>, getLocations, (Url), (const override));
};

#endif // TESTS_MOCKS_DATABASEMOCK_HPP_