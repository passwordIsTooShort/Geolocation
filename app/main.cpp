#include <algorithm>
#include <iostream>
#include <memory>
#include <QtWidgets/QApplication>

#include "SQLiteDatabase.hpp"
#include "OnlineLocationProvider.hpp"
#include "IpFromUrlProvider.hpp"
#include "BasicLocationManager.hpp"
#include "MainWindow.hpp"

int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <api_key> <database_path>\n";
        std::terminate();
    }
    const std::string accesKey = argv[1];
    const std::string databasePath = argv[2];

    DatabaseConfig databaseConfiguration(DatabaseConfig::DatabaseType::SQL_LITE, databasePath, "ip_locations");
    std::unique_ptr<IDatabase> database = std::make_unique<SQLiteDatabase>(databaseConfiguration);

    const std::string hostName = "http://api.ipstack.com";
    std::unique_ptr<ILocationProvider> locationProvider = std::make_unique<OnlineLocationProvider>(hostName, accesKey);

    std::unique_ptr<IIpFromUrlProvider> ipFromUrlProvider = std::make_unique<IpFromUrlProvider>();

    std::unique_ptr<ILocationManager> locationManager =
        std::make_unique<BasicLocationManager>(std::move(database),
                                               std::move(locationProvider),
                                               std::move(ipFromUrlProvider));

    MainWindow mainWindow(std::move(locationManager));
    mainWindow.setWindowTitle("Geolocation provider");
    mainWindow.show();

    return application.exec();
}