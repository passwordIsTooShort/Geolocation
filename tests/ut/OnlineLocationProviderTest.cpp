#include "OnlineLocationProvider.hpp"

#include "mocks/NetworkAccessHandlerMock.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class OnlineLocationProviderTest : public testing::Test
{
protected:
  const std::string TEST_HOST_NAME = "TEST_HOST_NAME";
  const std::string TEST_API_KEY = "TEST_API_KEY";
  const std::string ERROR_STRING = "ERROR_STRING";
  const std::string EXPECTED_STRING_FAIL_TO_PARSE_JSON = "JSON parsing error:";
  const std::string EXPECTED_STRING_WRONG_STATUS_CODE = "Received NOK response. Status code:";
  const std::string EXPECTED_STRING_NO_GEOLOCATION_IN_RESPONSE = "JSON do not contains geolocation data";
  const double TEST_LATITUDE = 10.101010;
  const double TEST_LONGITUDE = 32.323232;
  const int ERROR_STATUS_CODE = 404;
  const IpAddress TEST_IP_ADDRESS = IpAddress("1.1.1.1");

  struct TestConfig
  {
    NetworkReply testReply;

    bool expectedErrorCallback{false};
    std::string expectedErrorCallbackString{""};

    bool expectedSuccessCallback{false};
    GeolocationData expectedGeolocation{-1,-1};
  };

  std::string createJsonResponse(double latitude, double longitude) const;
  void runTest(TestConfig config);
};

MATCHER_P(GeolocationMatcher, expectedGeolocation, "")
{
  constexpr double MIN_DIFF_TO_TREAT_EQUAL = 0.00001;
  return abs(arg.latitude - expectedGeolocation.latitude) < MIN_DIFF_TO_TREAT_EQUAL &&
         abs(arg.longitude - expectedGeolocation.longitude) < MIN_DIFF_TO_TREAT_EQUAL;
}

std::string OnlineLocationProviderTest::createJsonResponse(double latitude, double longitude) const
{
  std::string reply("{\"latitude\": ");
  reply += std::to_string(latitude);
  reply += ", \"longitude\": ";
  reply += std::to_string(longitude);
  reply += "}";
  return reply;
}

void OnlineLocationProviderTest::runTest(TestConfig config)
{
  auto networkAccesHandlerMock = std::make_unique<testing::StrictMock<NetworkAccessHandlerMock>>();
  EXPECT_CALL(*networkAccesHandlerMock,
              get(testing::_, testing::_)).
                WillOnce(testing::InvokeArgument<1>(config.testReply));

  testing::MockFunction<void(std::string)> failureCallbackMock;
  if (config.expectedErrorCallback)
  {
    EXPECT_CALL(failureCallbackMock, Call(testing::HasSubstr(config.expectedErrorCallbackString)));
  }
  else
  {
    EXPECT_CALL(failureCallbackMock, Call(testing::_)).Times(0);
  }

  testing::MockFunction<void(GeolocationData)> successfulCallbackMock;
  if (config.expectedSuccessCallback)
  {
    EXPECT_CALL(successfulCallbackMock, Call(GeolocationMatcher(config.expectedGeolocation)));
  }
  else
  {
    EXPECT_CALL(successfulCallbackMock, Call(testing::_)).Times(0);
  }

  auto objectUnderTest = std::make_unique<OnlineLocationProvider>(std::move(networkAccesHandlerMock), TEST_HOST_NAME, TEST_API_KEY);
  objectUnderTest->getByIp(TEST_IP_ADDRESS, successfulCallbackMock.AsStdFunction(), failureCallbackMock.AsStdFunction());
}

TEST_F(OnlineLocationProviderTest, ConstructorDestructor)
{
  auto networkAccesHandlerMock = std::make_unique<testing::StrictMock<NetworkAccessHandlerMock>>();
  auto objectUnderTest = std::make_unique<OnlineLocationProvider>(std::move(networkAccesHandlerMock), TEST_HOST_NAME, TEST_API_KEY);
}

TEST_F(OnlineLocationProviderTest, SuccessfulyGetIp)
{
  std::string successfullReply = createJsonResponse(TEST_LATITUDE, TEST_LONGITUDE);
  NetworkReply testReply(successfullReply, NetworkReply::OK_STATUS_CODE, "");
  TestConfig testConfig{testReply};

  testConfig.expectedSuccessCallback = true;
  testConfig.expectedGeolocation = GeolocationData{TEST_LATITUDE, TEST_LONGITUDE};

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, FailToGetIp_emptyResponse)
{
  NetworkReply testReply("", NetworkReply::OK_STATUS_CODE, "");
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString = EXPECTED_STRING_FAIL_TO_PARSE_JSON;

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, FailToGetIp_errorResponse_emptyJson)
{
  NetworkReply testReply("", NetworkReply::OK_STATUS_CODE, ERROR_STRING);
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString = ERROR_STRING;

  runTest(testConfig);
}

// Note: In case of error, json shouldn't be parsed (even if it is correct)
TEST_F(OnlineLocationProviderTest, FailToGetIp_errorResponse_correctJson)
{
  std::string successfullReply = createJsonResponse(TEST_LATITUDE, TEST_LONGITUDE);
  NetworkReply testReply(successfullReply, NetworkReply::OK_STATUS_CODE, ERROR_STRING);
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString = ERROR_STRING;

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, FailToGetIp_errorStatusCode_emptyJson)
{
  NetworkReply testReply("", ERROR_STATUS_CODE, "");
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString =
    EXPECTED_STRING_WRONG_STATUS_CODE + " " + std::to_string(ERROR_STATUS_CODE);

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, FailToGetIp_errorStatusCode_correctJson)
{
  std::string successfullReply = createJsonResponse(TEST_LATITUDE, TEST_LONGITUDE);
  NetworkReply testReply(successfullReply, ERROR_STATUS_CODE, "");
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString =
    EXPECTED_STRING_WRONG_STATUS_CODE + " " + std::to_string(ERROR_STATUS_CODE);

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, FailToGetIp_bothErrorAndWrongStatusCode)
{
  NetworkReply testReply("", ERROR_STATUS_CODE, ERROR_STRING);
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString = ERROR_STRING;

  runTest(testConfig);
}

TEST_F(OnlineLocationProviderTest, ResponseReceivedButWithoutGeolocation)
{
  std::string reply("{\"location_a\": ");
  reply += std::to_string(TEST_LATITUDE);
  reply += ", \"location_b\": ";
  reply += std::to_string(TEST_LONGITUDE);
  reply += "}";

  NetworkReply testReply(reply, NetworkReply::OK_STATUS_CODE, "");
  TestConfig testConfig{testReply};

  testConfig.expectedErrorCallback = true;
  testConfig.expectedErrorCallbackString = EXPECTED_STRING_NO_GEOLOCATION_IN_RESPONSE;

  runTest(testConfig);
}