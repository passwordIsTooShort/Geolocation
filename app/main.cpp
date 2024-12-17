#include <iostream>
#include <memory>
#include <QtWidgets/QApplication>

#include "SqlDatabase.hpp"
#include "OnlineLocationProvider.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <api_key>\n";
        std::terminate();
    }
    const std::string accesKey = argv[1];

    QApplication application(argc, argv);

    std::unique_ptr<IDatabase> database = std::make_unique<SqlDatabase>();
    std::cout << "Connect to database result: " << database->connectToDatabase() << '\n';

    const std::string hostName = "http://api.ipstack.com";
    std::unique_ptr<ILocationProvider> locationProvider = std::make_unique<OnlineLocationProvider>(hostName, accesKey);
        
    auto callback = [](std::string ip, std::string apiKey, GeolocationData geolocation)
    {
        const std::size_t charsToBePrintedFrontAndBack = 6;
        const std::size_t replaceStartPoint = charsToBePrintedFrontAndBack;
        const std::size_t elementsToReplace = apiKey.size() - replaceStartPoint - charsToBePrintedFrontAndBack;
        auto strippedApiKey = apiKey.replace(replaceStartPoint, elementsToReplace, elementsToReplace, '*');
        std::cout << "Geolocation for ip: " << ip << ", requested from api key: " << strippedApiKey << "\n";
        std::cout << "Latitiude: " << geolocation.getLatitude() << "\n";
        std::cout << "Longitude: " << geolocation.geLongitude() << "\n";
    };
    locationProvider->setNewGeolocationCallback(std::move(callback));

    locationProvider->getByIp(IpAdress("134.201.250.155"));


    return application.exec();
}