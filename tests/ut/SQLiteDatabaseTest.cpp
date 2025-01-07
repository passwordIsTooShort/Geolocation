#include "intf/IDatabase.hpp"
#include "SQLiteDatabase.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class SQLiteDatabaseTest : public testing::Test
{
public:
    /**
     * @brief Constructor
     */
    SQLiteDatabaseTest() = default;

    /**
     * @brief Destructor
     */
    ~SQLiteDatabaseTest() override = default;
};

TEST_F(SQLiteDatabaseTest, Constructor)
{

}