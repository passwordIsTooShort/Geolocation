#include <iostream>
#include <variant>

#include "BasicLocationManager.hpp"

BasicLocationManager::BasicLocationManager(std::unique_ptr<IDatabase> database,
                        std::unique_ptr<ILocationProvider> locationProvider,
                        std::unique_ptr<IIpFromUrlProvider> ipFromUrlProvider)
: mDatabase{std::move(database)}
, mLocationProvider{std::move(locationProvider)}
, mIpFromUrlProvider{std::move(ipFromUrlProvider)}
{
    std::cout << "Connect to database result: " << mDatabase->connectToDatabase() << '\n';
    std::cout << "Table creation result: " << mDatabase->prepareToUse() << '\n';
}

void BasicLocationManager::addLocation(IpAddress ipAddress)
{
    insideAddLocation(ipAddress);
}

void BasicLocationManager::addLocation(Url url)
{
    insideAddLocation(url);
}

void BasicLocationManager::updateLocation(IpAddress ipAddress)
{
    // TODO: Verify if correct IP address
    startProcessing(ipAddress);
    auto successCallback = [this, ipAddress](auto geolocationData)
    {
        handleNewLocation(ipAddress, geolocationData);
    };
    auto failureCallback = [this, ipAddress](auto errorString)
    {
        finishedWithFail(ipAddress);
        std::cout << "Failed to update location of IP: "
                    << ipAddress.toString()
                    << ". Error: "
                    << errorString;
    };

    mLocationProvider->getByIp(ipAddress, successCallback, failureCallback);
}

void BasicLocationManager::updateLocation(Url url)
{
    // TODO: Verfy url
    startProcessing(url);
    auto successCallback = [this, url](auto ipAddresses)
    {
        for (const auto& signleIp : ipAddresses)
        {
            mIpToUrlMap.emplace(signleIp, url);
            updateLocation(signleIp);
        }
    };
    auto failureCallback = [this, url](auto failureInfo)
    {
        finishedWithFail(url);
        std::cout << "Failed to get ip for url: " << url.toString()
                  << ". Reason: " << failureInfo << '\n';
    };

    mIpFromUrlProvider->getIpForUrl(url, successCallback, failureCallback);
}

ILocationManager::LocationStatus BasicLocationManager::getLocationStatus(IpAddress ipAddress)
{
    auto result = ILocationManager::LocationStatus::NOT_EXIST;
    if (isInProcesingList(ipAddress))
    {
        result = ILocationManager::LocationStatus::IN_PROGRESS;
    }
    else if (isMarkedAsFailed(ipAddress))
    {
        result = ILocationManager::LocationStatus::FAILED_TO_GET_LOCATION;
    }
    else if(mDatabase->getLocations(ipAddress).size() > 0)
    {
        result = ILocationManager::LocationStatus::READY_TO_READ;
    }

    return result;
}

ILocationManager::LocationStatus BasicLocationManager::getLocationStatus(Url url)
{
    auto result = ILocationManager::LocationStatus::NOT_EXIST;
    if (isInProcesingList(url))
    {
        result = ILocationManager::LocationStatus::IN_PROGRESS;
    }
    else if (isMarkedAsFailed(url))
    {
        result = ILocationManager::LocationStatus::FAILED_TO_GET_IP;
    }
    else if(mDatabase->getLocations(url).size() > 0)
    {
        result = ILocationManager::LocationStatus::READY_TO_READ;
    }

    return result;
}

std::vector<IpLocationData> BasicLocationManager::getLocations(IpAddress ipAddress)
{
    return mDatabase->getLocations(ipAddress);
}

std::vector<IpLocationData> BasicLocationManager::getLocations(Url url)
{
    return mDatabase->getLocations(url);
}

void BasicLocationManager::startProcessing(std::variant<IpAddress, Url> ipOrUrl)
{
    mProcessingElements.insert(ipOrUrl);
}

void BasicLocationManager::finishedSuccesfully(std::variant<IpAddress, Url> ipOrUrl)
{
    mProcessingElements.erase(ipOrUrl);
}

void BasicLocationManager::finishedWithFail(std::variant<IpAddress, Url> ipOrUrl)
{
    mProcessingElements.erase(ipOrUrl);
    mFailedElements.insert(ipOrUrl);
}

bool BasicLocationManager::isInProcesingList(std::variant<IpAddress, Url> ipOrUrl) const
{
    const auto elementIter = mProcessingElements.find(ipOrUrl);
    return elementIter != mProcessingElements.end();
}

bool BasicLocationManager::isMarkedAsFailed(std::variant<IpAddress, Url> ipOrUrl) const
{
    const auto elementIter = mFailedElements.find(ipOrUrl);
    return elementIter != mFailedElements.end();
}

void BasicLocationManager::handleNewLocation(IpAddress ipAddress,
                                             GeolocationData geolocationData)
{
    IpLocationData newLocation = {geolocationData, ipAddress};

    auto urlIter = mIpToUrlMap.find(ipAddress);
    if (urlIter != mIpToUrlMap.end())
    {
        newLocation.url = urlIter->second;
        mIpToUrlMap.erase(urlIter);
    }
    mDatabase->add(newLocation);
    mNewLocationCallback(newLocation);

    finishedSuccesfully(ipAddress);
    if (!newLocation.url.isEmpty())
    {
        finishedSuccesfully(newLocation.url);
    }
}
