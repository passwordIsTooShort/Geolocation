#include "Application.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <QtWidgets/QApplication>

#include "intf/INetworkAccessHandler.hpp"
#include "NetworkAccessHandler.hpp"
#include "SQLiteDatabase.hpp"
#include "OnlineLocationProvider.hpp"
#include "IpFromUrlProvider.hpp"
#include "BasicLocationManager.hpp"
#include "MainWindow.hpp"

std::optional<AppConfigurationData> parseConfigFromConsoleArgList(int argc, char **argv);
void recreateLocationManager(std::unique_ptr<ILocationManager>& locationManager, AppConfigurationData configuration);

int run(int argc, char **argv)
{
    QApplication application(argc, argv);

    std::unique_ptr<ILocationManager> locationManager = nullptr;
    auto inputAppConfiguration = parseConfigFromConsoleArgList(argc, argv);
    if (inputAppConfiguration)
    {
        recreateLocationManager(locationManager, inputAppConfiguration.value());
    }

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Geolocation provider");
    mainWindow.show();

    QObject::connect(&mainWindow, &MainWindow::saveConfiguration,
    [&locationManager, &mainWindow](auto configuration)
    {
        recreateLocationManager(locationManager, configuration);
        QObject::connect(static_cast<BasicLocationManager*>(locationManager.get()), &BasicLocationManager::newLocation,
                         &mainWindow, &MainWindow::onNewLocation);
    });

    QObject::connect(&mainWindow, &MainWindow::requestToRemoveLocation,
    [&locationManager, &mainWindow](QString ipOrUrl)
    {
        if (!locationManager)
        {
            mainWindow.showDialog("Cannot remove location: " + ipOrUrl + ". Provide first app configuration (file->Configuration)");
        }
        else if (ILocationManager::LocationStatus::READY_TO_READ != locationManager->getLocationStatus(ipOrUrl.toStdString()))
        {
            mainWindow.showDialog("Location: " + ipOrUrl + " do not exist in database. Cannot remove it");
        }
        else if (!locationManager->removeLocation(ipOrUrl.toStdString()))
        {
            mainWindow.showDialog("Failed to remove location: " + ipOrUrl);
        }
        else if (ILocationManager::LocationStatus::NOT_EXIST != locationManager->getLocationStatus(ipOrUrl.toStdString()))
        {
            mainWindow.showDialog("Location: " + ipOrUrl + " removed, but still exists in DB. Internal error");
        }
        else
        {
            mainWindow.showDialog("Location: " + ipOrUrl + " succesfully removed", MainWindow::DialogLevel::INFO);
        }
    });

    QObject::connect(&mainWindow, &MainWindow::requestToGetLocation,
    [&locationManager, &mainWindow](QString ipOrUrl, LocationAddConfig config)
    {
        if (locationManager)
        {
            if (config.forceUpdate)
            {
                locationManager->updateLocation(ipOrUrl.toStdString());
                return;
            }

            const auto resultsList = locationManager->getLocations(ipOrUrl.toStdString());
            if (resultsList.empty())
            {
                if (config.searchOnlyDb)
                {
                    QString error = "Failed to get location for: " %
                                     QString::fromStdString(ipOrUrl.toStdString()) %
                                     ". Location not available in DB, and \"only DB\" is checked in configuration";
                    mainWindow.showDialog(error);
                }
                else
                {
                    locationManager->addLocation(ipOrUrl.toStdString());
                }
            }
            else
            {
                for (auto&& singleLocatin : resultsList)
                {
                    mainWindow.onNewLocation(singleLocatin);
                }
            }
        }
        else
        {
            mainWindow.showDialog("Configuration for application was not provided - fill up first configuration (file->Configuration)");
        }
    });

    return application.exec();
}

std::optional<AppConfigurationData> parseConfigFromConsoleArgList(int argc, char **argv)
{
    if (argc == 4)
    {
        const auto databaseType = DatabaseConfig::DatabaseType::SQL_LITE;
        const std::string accesKey = argv[1];
        const std::string databasePath = argv[2];
        const std::string tableName = argv[3];

        DatabaseConfig databaseConfig{databaseType, databasePath, tableName};
        return std::optional<AppConfigurationData>(AppConfigurationData{databaseConfig, accesKey});
    }
    return std::nullopt;
}

void recreateLocationManager(std::unique_ptr<ILocationManager>& locationManager, AppConfigurationData configuration)
{
    const std::string hostName = "http://api.ipstack.com";
    std::unique_ptr<INetworkAccessHandler> networkAccessManager = std::make_unique<NetworkAccessHandler>();
    std::unique_ptr<ILocationProvider> locationProvider =
        std::make_unique<OnlineLocationProvider>(std::move(networkAccessManager), hostName, configuration.apiKey);
    std::unique_ptr<IIpFromUrlProvider> ipFromUrlProvider = std::make_unique<IpFromUrlProvider>();
    std::unique_ptr<IDatabase> database = std::make_unique<SQLiteDatabase>(configuration.databaseConfig);

    locationManager = std::make_unique<BasicLocationManager>(std::move(database),
                                            std::move(locationProvider),
                                            std::move(ipFromUrlProvider));
}