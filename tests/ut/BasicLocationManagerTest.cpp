#include "BasicLocationManager.hpp"

#include "mocks/DatabaseMock.hpp"
#include "mocks/IpFromUrlProviderMock.hpp"
#include "mocks/LocationProviderMock.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

MATCHER_P(IpUrlMatcher, other, "")
{
  return arg.toString() == other.toString();
}

class BasicLocationManagerTest : public testing::Test
{
protected:
  // const std::string TEST_HOST_NAME = "TEST_HOST_NAME";
  // const std::string TEST_API_KEY = "TEST_API_KEY";
  const std::string ERROR_STRING = "ERROR_STRING";
  // const std::string EXPECTED_STRING_FAIL_TO_PARSE_JSON = "JSON parsing error:";
  // const std::string EXPECTED_STRING_WRONG_STATUS_CODE = "Received NOK response. Status code:";
  // const std::string EXPECTED_STRING_NO_GEOLOCATION_IN_RESPONSE = "JSON do not contains geolocation data";
  const double TEST_LATITUDE = 10.101010;
  const double TEST_LONGITUDE = 32.323232;
  const GeolocationData TEST_GEOLOCATION_DATA = {TEST_LATITUDE, TEST_LONGITUDE};
  const IpAddress TEST_IP_ADDRESS = IpAddress("1.1.1.1");
  const Url TEST_URL = Url("this_is_test_url");
  const IpLocationData TEST_IP_LOCATION_DATA = {TEST_GEOLOCATION_DATA, TEST_IP_ADDRESS, TEST_URL};


  auto GeolocationMatcher(const GeolocationData& expected)
  {
    return testing::AllOf(
           testing::Field(&GeolocationData::latitude, testing::DoubleNear(expected.latitude, 0.0001)),
           testing::Field(&GeolocationData::longitude, testing::DoubleNear(expected.longitude, 0.0001))
      );
  }

  auto IpLocationMatcher(const IpLocationData& expected)
  {
    return testing::AllOf(
      testing::Field(&IpLocationData::geolocationdata, GeolocationMatcher(expected.geolocationdata)),
      testing::Field(&IpLocationData::ipAddress, IpUrlMatcher(expected.ipAddress)),
      testing::Field(&IpLocationData::url, IpUrlMatcher(expected.url))
    );
  }

  // auto IpMatcher(const GeolocationData& expected)
  // {
  //   return testing::AllOf(
  //          testing::Field(&GeolocationData::latitude, testing::DoubleNear(expected.latitude, 0.0001)),
  //          testing::Field(&GeolocationData::longitude, testing::DoubleNear(expected.longitude, 0.0001))
  //     );
  // }

};

// MATCHER_P(GeolocationMatcher, expectedGeolocation, "")
// {
//   constexpr double MIN_DIFF_TO_TREAT_EQUAL = 0.00001;
//   return abs(arg.latitude - expectedGeolocation.latitude) < MIN_DIFF_TO_TREAT_EQUAL &&
//          abs(arg.longitude - expectedGeolocation.longitude) < MIN_DIFF_TO_TREAT_EQUAL;
// }

// std::string OnlineLocationProviderTest::createJsonResponse(double latitude, double longitude) const
// {
//   std::string reply("{\"latitude\": ");
//   reply += std::to_string(latitude);
//   reply += ", \"longitude\": ";
//   reply += std::to_string(longitude);
//   reply += "}";
//   return reply;
// }

// void OnlineLocationProviderTest::runTest(TestConfig config)
// {
//   auto networkAccesHandlerMock = std::make_unique<testing::StrictMock<NetworkAccessHandlerMock>>();
//   EXPECT_CALL(*networkAccesHandlerMock,
//               get(testing::_, testing::_)).
//                 WillOnce(testing::InvokeArgument<1>(config.testReply));

//   testing::MockFunction<void(std::string)> failureCallbackMock;
//   if (config.expectedErrorCallback)
//   {
//     EXPECT_CALL(failureCallbackMock, Call(testing::HasSubstr(config.expectedErrorCallbackString)));
//   }
//   else
//   {
//     EXPECT_CALL(failureCallbackMock, Call(testing::_)).Times(0);
//   }

//   testing::MockFunction<void(GeolocationData)> successfulCallbackMock;
//   if (config.expectedSuccessCallback)
//   {
//     EXPECT_CALL(successfulCallbackMock, Call(GeolocationMatcher(config.expectedGeolocation)));
//   }
//   else
//   {
//     EXPECT_CALL(successfulCallbackMock, Call(testing::_)).Times(0);
//   }

//   auto objectUnderTest = std::make_unique<OnlineLocationProvider>(std::move(networkAccesHandlerMock), TEST_HOST_NAME, TEST_API_KEY);
//   objectUnderTest->getByIp(TEST_IP_ADDRESS, successfulCallbackMock.AsStdFunction(), failureCallbackMock.AsStdFunction());
// }

TEST_F(BasicLocationManagerTest, ConstructorDestructor_successfullyCreated)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
}

TEST_F(BasicLocationManagerTest, ConstructorDestructor_failedToConnectToDb)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(false));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::DATABASE_ERROR);
}

TEST_F(BasicLocationManagerTest, ConstructorDestructor_failedToPrepareDb)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(false));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::DATABASE_ERROR);
}

//////// Adding by IP ////////
TEST_F(BasicLocationManagerTest, addIp_successfull)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS))));
  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(TEST_IP_ADDRESS), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<1>(TEST_GEOLOCATION_DATA));

  IpLocationData expectedIpLocation{TEST_GEOLOCATION_DATA, TEST_IP_ADDRESS};
  EXPECT_CALL(*databaseMock, add(IpLocationMatcher(expectedIpLocation)));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_IP_ADDRESS.toString());
}

TEST_F(BasicLocationManagerTest, addIp_failed)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS))));
  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(TEST_IP_ADDRESS), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<2>(ERROR_STRING));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_IP_ADDRESS.toString());
  EXPECT_EQ(objectUnderTest->getLocationStatus(TEST_IP_ADDRESS.toString()), ILocationManager::LocationStatus::FAILED_TO_GET_LOCATION);
}

TEST_F(BasicLocationManagerTest, addIp_alreadyInProgress)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS))));
  // Prevent calling callback will cause, that IP will be processed forever
  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(TEST_IP_ADDRESS), testing::_, testing::_));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_IP_ADDRESS.toString());
  EXPECT_EQ(objectUnderTest->getLocationStatus(TEST_IP_ADDRESS.toString()), ILocationManager::LocationStatus::IN_PROGRESS);
}

//////// Adding by URL ////////
TEST_F(BasicLocationManagerTest, addUrl_urlWithOneIp_successfull)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<Url>(IpUrlMatcher(TEST_URL))));

  // Seems that callbacks with r-value parameters are not working with Gtest. Using workaround:
  // https://stackoverflow.com/questions/70885968/gtest-compiler-error-when-invoking-function-with-rvalue-argument
  EXPECT_CALL(*ipFromUrlProviderMock, getIpForUrl(IpUrlMatcher(TEST_URL), testing::_, testing::_)).
    WillOnce(testing::WithArg<1>(
      [this](std::function<void(std::vector<IpAddress>&& ip)> callback)
      {
        callback(std::vector<IpAddress>{TEST_IP_ADDRESS});
      }
    ));

  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(TEST_IP_ADDRESS), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<1>(TEST_GEOLOCATION_DATA));

  IpLocationData expectedIpLocation{TEST_GEOLOCATION_DATA, TEST_IP_ADDRESS, TEST_URL};
  EXPECT_CALL(*databaseMock, add(IpLocationMatcher(expectedIpLocation)));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_URL.toString());
}

TEST_F(BasicLocationManagerTest, addUrl_urlWithMultipleIp_successfull)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<Url>(IpUrlMatcher(TEST_URL))));

  const auto testIpAddress2 = IpAddress("22.22.22.22");
  const auto testGeolocation2 = GeolocationData{99.99, -89.89};
  const auto testIpAddress3 = IpAddress("100.100.100.100");
  const auto testGeolocation3 = GeolocationData{0.99, -0.89};

  // Seems that callbacks with r-value parameters are not working with Gtest. Using workaround:
  // https://stackoverflow.com/questions/70885968/gtest-compiler-error-when-invoking-function-with-rvalue-argument
  EXPECT_CALL(*ipFromUrlProviderMock, getIpForUrl(IpUrlMatcher(TEST_URL), testing::_, testing::_)).
    WillOnce(testing::WithArg<1>(
      [this, testIpAddress2, testIpAddress3](std::function<void(std::vector<IpAddress>&& ip)> callback)
      {
        callback(std::vector<IpAddress>{TEST_IP_ADDRESS, testIpAddress2, testIpAddress3});
      }
    ));

  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(TEST_IP_ADDRESS), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<1>(TEST_GEOLOCATION_DATA));
  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(testIpAddress2), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<1>(testGeolocation2));
  EXPECT_CALL(*locationProviderMock, getByIp(IpUrlMatcher(testIpAddress3), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<1>(testGeolocation3));

  EXPECT_CALL(*databaseMock, add(IpLocationMatcher(TEST_IP_LOCATION_DATA)));

  IpLocationData expectedIpLocation2{testGeolocation2, testIpAddress2, TEST_URL};
  EXPECT_CALL(*databaseMock, add(IpLocationMatcher(expectedIpLocation2)));

  IpLocationData expectedIpLocation3{testGeolocation3, testIpAddress3, TEST_URL};
  EXPECT_CALL(*databaseMock, add(IpLocationMatcher(expectedIpLocation3)));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_URL.toString());
}

TEST_F(BasicLocationManagerTest, addUrl_failedToGetIp)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<Url>(IpUrlMatcher(TEST_URL))));

  EXPECT_CALL(*ipFromUrlProviderMock, getIpForUrl(IpUrlMatcher(TEST_URL), testing::_, testing::_)).
    WillOnce(testing::InvokeArgument<2>(ERROR_STRING));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);

  objectUnderTest->addLocation(TEST_URL.toString());
  EXPECT_EQ(objectUnderTest->getLocationStatus(TEST_URL.toString()), ILocationManager::LocationStatus::FAILED_TO_GET_IP);
}

//////// Removing by IP ////////
TEST_F(BasicLocationManagerTest, removeByIp_successfull)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, remove(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS)))).
    WillOnce(testing::Return(true));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_TRUE(objectUnderTest->removeLocation(TEST_IP_ADDRESS.toString()));
}

TEST_F(BasicLocationManagerTest, removeByIp_failure)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, remove(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS)))).
    WillOnce(testing::Return(false));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_FALSE(objectUnderTest->removeLocation(TEST_IP_ADDRESS.toString()));
}

//////// Removing by URL ////////
TEST_F(BasicLocationManagerTest, removeByUrl_successfull)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, remove(testing::Matcher<Url>(IpUrlMatcher(TEST_URL)))).
    WillOnce(testing::Return(true));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_TRUE(objectUnderTest->removeLocation(TEST_URL.toString()));
}

TEST_F(BasicLocationManagerTest, removeByUrl_failure)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, remove(testing::Matcher<Url>(IpUrlMatcher(TEST_URL)))).
    WillOnce(testing::Return(false));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_FALSE(objectUnderTest->removeLocation(TEST_URL.toString()));
}

//////// Get location status (only missing scenarios) ////////
TEST_F(BasicLocationManagerTest, getLocationStatusOfIp_locationNotInDb)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<IpAddress>(IpUrlMatcher(TEST_IP_ADDRESS)))).
    WillOnce(testing::Return(std::vector<IpLocationData>{}));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_EQ(objectUnderTest->getLocationStatus(TEST_IP_ADDRESS.toString()), ILocationManager::LocationStatus::NOT_EXIST);
}

TEST_F(BasicLocationManagerTest, getLocationStatusOfUrl_locationNotInDb)
{
  auto databaseMock = std::make_unique<testing::StrictMock<DatabaseMock>>();
  auto locationProviderMock = std::make_unique<testing::StrictMock<LocationProviderMock>>();
  auto ipFromUrlProviderMock = std::make_unique<testing::StrictMock<IpFromUrlProviderMock>>();
  EXPECT_CALL(*databaseMock, connectToDatabase()).WillOnce(testing::Return(true));
  EXPECT_CALL(*databaseMock, prepareToUse()).WillOnce(testing::Return(true));

  EXPECT_CALL(*databaseMock, getLocations(testing::Matcher<Url>(IpUrlMatcher(TEST_URL)))).
    WillOnce(testing::Return(std::vector<IpLocationData>{}));

  auto objectUnderTest = std::make_unique<BasicLocationManager>(std::move(databaseMock),
                                                                std::move(locationProviderMock),
                                                                std::move(ipFromUrlProviderMock));

  EXPECT_EQ(objectUnderTest->getStatus(), ILocationManager::ManagerStatus::READY_TO_USE);
  EXPECT_EQ(objectUnderTest->getLocationStatus(TEST_URL.toString()), ILocationManager::LocationStatus::NOT_EXIST);
}