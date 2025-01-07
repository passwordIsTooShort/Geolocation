#include "OnlineLocationProvider.hpp"

#include "mocks/NetworkAccessManagerMock.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class OnlineLocationProviderTest : public testing::Test
{
public:
    /**
     * @brief Constructor
     */
    OnlineLocationProviderTest()
    // : mNetworkAccesManagerMock{std::make_unique<NetworkAccessManagerMock>()}
    {
    }

    /**
     * @brief Destructor
     */
    ~OnlineLocationProviderTest() override = default;

protected:
  const std::string TEST_HOST_NAME = "TEST_HOST_NAME";
  const std::string TEST_API_KEY = "TEST_API_KEY";

  // std::unique_ptr<INetworkAccessManager> mNetworkAccesManagerMock;

  std::unique_ptr<ILocationProvider> createObjectUnderTest();
};

std::unique_ptr<ILocationProvider> OnlineLocationProviderTest::createObjectUnderTest()
{
  std::unique_ptr<INetworkAccessManager> networkAccesManagerMock = std::make_unique<NetworkAccessManagerMock>();
  return std::make_unique<OnlineLocationProvider>(std::move(networkAccesManagerMock), TEST_HOST_NAME, TEST_API_KEY);
}

TEST_F(OnlineLocationProviderTest, ConstructorDestructor)
{
  auto objectUnderTest = createObjectUnderTest();
}

// TEST_F(OnlineLocationProviderTest, ConstructorDestructor)
// {
//   auto objectUnderTest = createObjectUnderTest();
// }