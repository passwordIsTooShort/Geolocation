#include <iostream>

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

    auto onNewLocationCallback = [this](IpAddress ip, std::string apiKey, GeolocationData geolocation)
    {
        const std::size_t charsToBePrintedFrontAndBack = 6;
        const std::size_t replaceStartPoint = charsToBePrintedFrontAndBack;
        const std::size_t elementsToReplace = apiKey.size() - replaceStartPoint - charsToBePrintedFrontAndBack;
        const auto strippedApiKey = apiKey.replace(replaceStartPoint, elementsToReplace, elementsToReplace, '*');

        auto urlIter = mIpToUrlMap.find(ip);
        if (urlIter != mIpToUrlMap.end())
        {
            auto url = *urlIter;
            // TODO: Add option to store in db also URL (optional field)
            //mDatabase->add(geolocation, ip, url);
            mIpToUrlMap.erase(urlIter);
        }
        else
        {
            mDatabase->add(geolocation, ip);
        }
        mNewLocationCallback(ip, strippedApiKey, geolocation);

        auto finishedIpIter = mProcessingIps.find(ip);
        if (finishedIpIter != mProcessingIps.end())
        {
            mProcessingIps.erase(finishedIpIter);
        }
        else
        {
            std::cout << "Ip : " << ip.getIpAddress() << " was processed, but wasn't on requests list\n";
        }   
   };

   mLocationProvider->setOnNewGeolocationCallback(std::move(onNewLocationCallback));
}

void BasicLocationManager::addLocationOfIp(IpAddress ipAddress)
{
    const auto ipLocationStatus = getIpLocationStatus(ipAddress);
    if (ipLocationStatus == LocationStatus::READY_TO_READ)
    {
        std::cout << "Ip address: " << ipAddress.getIpAddress() << " already exists in database."
                  << " To force update it, run: method updateLocationOfIp.\n";
        return;
    }
    else if(ipLocationStatus == LocationStatus::IN_PROGRESS)
    {
        std::cout << "Ip address: " << ipAddress.getIpAddress() << " is currently processing."
                  << " Wait to finish this request.\n";
        return;
    }

    updateLocationOfIp(ipAddress);
}

void BasicLocationManager::addLocationOfUrl(std::string url)
{
    const auto urlLocationStatus = getUrlLocationStatus(url);
    if (urlLocationStatus == LocationStatus::READY_TO_READ ||
        urlLocationStatus == LocationStatus::IN_PROGRESS)
    {
        std::cout << "Url: " << url << " already exists in database"
                  << "or is processing. To force update it, run: method updateLocationOfUrl"
                  << ". Skipping this request\n";
        return;
    }

    updateLocationOfUrl(url);
}

void BasicLocationManager::updateLocationOfIp(IpAddress ipAddress)
{
    // TODO: Verify if correct IP address

    mProcessingIps.insert(ipAddress);
    mLocationProvider->getByIp(ipAddress);
}
    
void BasicLocationManager::updateLocationOfUrl(std::string url)
{
    // TODO: Verfy url

    auto successCallback = [this](auto url, auto ipAddresses)
    {
        for (const auto& signleIp : ipAddresses)
        {
            mIpToUrlMap[signleIp] = url;
            updateLocationOfIp(signleIp);
        }
    };

    auto failureCallback = [this](auto url, auto failureInfo)
    {
        addFailedUrl(url);
        std::cout << "Failed to get ip for url: " << url
                  << ". Reason: " << failureInfo << '\n';
    };

    mIpFromUrlProvider->getIpForUrl(url, successCallback, failureCallback);
}

ILocationManager::LocationStatus BasicLocationManager::getIpLocationStatus(IpAddress ipAddress)
{
    auto result = ILocationManager::LocationStatus::NOT_EXIST;
    if (isIpInProcesingList(ipAddress))
    {
        result = ILocationManager::LocationStatus::IN_PROGRESS;
    }
    else if (isIpInFailedList(ipAddress))
    {
        result = ILocationManager::LocationStatus::FAILED_TO_GET_LOCATION;
    }
    else if(mDatabase->getLocation(ipAddress))
    {
        result = ILocationManager::LocationStatus::READY_TO_READ;
    }

    return result;
}

ILocationManager::LocationStatus BasicLocationManager::getUrlLocationStatus(std::string url)
{
    // TODO:
    (void) url;
    return ILocationManager::LocationStatus::NOT_EXIST;
}

std::optional<GeolocationData> BasicLocationManager::getLocationOfIp(IpAddress ipAddress)
{
    return mDatabase->getLocation(ipAddress);
}

std::optional<GeolocationData> BasicLocationManager::getLocationOfUrl(std::string url)
{
    (void) url;
    return std::nullopt;
}

bool BasicLocationManager::isIpInProcesingList(IpAddress ip) const
{
    auto ipIter = mProcessingIps.find(ip);
    return ipIter != mProcessingIps.end();
}

void BasicLocationManager::addFailedIp(IpAddress ip)
{
    // Add some max size, and if too many fails, just drop oldest one
    mIpsFailedToGetLocation.insert(ip);
}

bool BasicLocationManager::isIpInFailedList(IpAddress ip) const
{
    auto ipIter = mIpsFailedToGetLocation.find(ip);
    return ipIter != mIpsFailedToGetLocation.end();
}

void BasicLocationManager::addFailedUrl(std::string url)
{
    // Add some max size, and if too many fails, just drop oldest one
    mUrlsFailedToGetIp.insert(url);
}

bool BasicLocationManager::isUrlInFailedList(std::string url) const
{
    auto urlIter = mUrlsFailedToGetIp.find(url);
    return urlIter != mUrlsFailedToGetIp.end();
}
