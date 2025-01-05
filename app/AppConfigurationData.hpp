#ifndef APP_APPCONFIGURATIONDATA_HPP_
#define APP_APPCONFIGURATIONDATA_HPP_

#include "DatabaseConfig.hpp"

struct AppConfigurationData
{
    DatabaseConfig databaseConfig;
    std::string apiKey;
};

#endif // APP_APPCONFIGURATIONDATA_HPP_