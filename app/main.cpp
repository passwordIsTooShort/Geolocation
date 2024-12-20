#include <algorithm>
#include <iostream>
#include <memory>
#include <QtWidgets/QApplication>

#include "SQLiteDatabase.hpp"
#include "OnlineLocationProvider.hpp"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <api_key> <database_path>\n";
        std::terminate();
    }
    const std::string accesKey = argv[1];
    const std::string databasePath = argv[2];

    QApplication application(argc, argv);

    DatabaseConfig databaseConfiguration(DatabaseConfig::DatabaseType::SQL_LITE, databasePath);
    std::unique_ptr<IDatabase> database = std::make_unique<SQLiteDatabase>(databaseConfiguration);

    std::cout << "Connect to database result: " << database->connectToDatabase() << '\n';
    std::cout << "Table creation result: " << database->prepareToUse() << '\n';

    const std::string hostName = "http://api.ipstack.com";
    std::unique_ptr<ILocationProvider> locationProvider = std::make_unique<OnlineLocationProvider>(hostName, accesKey);

    auto callback = [&database](IpAdress ip, std::string apiKey, GeolocationData geolocation)
    {
        const std::size_t charsToBePrintedFrontAndBack = 6;
        const std::size_t replaceStartPoint = charsToBePrintedFrontAndBack;
        const std::size_t elementsToReplace = apiKey.size() - replaceStartPoint - charsToBePrintedFrontAndBack;
        const auto strippedApiKey = apiKey.replace(replaceStartPoint, elementsToReplace, elementsToReplace, '*');
        database->add(geolocation, ip);

        // Temporary test code:
        const auto location = database->getLocation(ip);
        if (location)
        {
            const auto& retreivedLocation = location.value();
            std::cout << "Location for ip: " << ip.getIpAddress() << " is: ("
                    << retreivedLocation.geLongitude() << ", "
                    << retreivedLocation.getLatitude() << ")\n";
        }
        else
        {
            std::cout << "Failed to get location for: " << ip.getIpAddress() << '\n';
        }
   };
    locationProvider->setOnNewGeolocationCallback(std::move(callback));

    IpAdress testAddress("134.201.250.155");
    locationProvider->getByIp(testAddress);

    return application.exec();
}