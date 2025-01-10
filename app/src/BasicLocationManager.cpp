#include "BasicLocationManager.hpp"

BasicLocationManager::BasicLocationManager(std::unique_ptr<IDatabase> database,
                        std::unique_ptr<ILocationProvider> locationProvider,
                        std::unique_ptr<IIpFromUrlProvider> ipFromUrlProvider)
: mDatabase{std::move(database)}
, mLocationProvider{std::move(locationProvider)}
, mIpFromUrlProvider{std::move(ipFromUrlProvider)}
{
    if (!mDatabase->connectToDatabase())
    {
        std::cerr << "Failed to connect to DB" << std::endl;
        mManagerStatus = ManagerStatus::DATABASE_ERROR;
    }

    if (!mDatabase->prepareToUse())
    {
        std::cerr << "Failed to prepare DB to use" << std::endl;
        mManagerStatus = ManagerStatus::DATABASE_ERROR;
    }

    if (mManagerStatus != ManagerStatus::DATABASE_ERROR)
    {
        mManagerStatus = ManagerStatus::READY_TO_USE;
    }
}

ILocationManager::ManagerStatus BasicLocationManager::getStatus() const
{
    return mManagerStatus;
}

void BasicLocationManager::addLocation(std::string ipOrUrl)
{
    if (IpAddress::isIpAddress(ipOrUrl))
    {
        addLocation(IpAddress(ipOrUrl));
    }
    else if (Url::isUrl(ipOrUrl))
    {
        addLocation(Url(ipOrUrl));
    }
    else
    {
        std::cout << "Provided string is neither ip nor url\n";
    }
}

void BasicLocationManager::updateLocation(std::string ipOrUrl)
{
    if (IpAddress::isIpAddress(ipOrUrl))
    {
        updateLocation(IpAddress(ipOrUrl));
    }
    else if (Url::isUrl(ipOrUrl))
    {
        updateLocation(Url(ipOrUrl));
    }
    else
    {
        std::cout << "Provided string is neither ip nor url\n";
    }
}

bool BasicLocationManager::removeLocation(std::string ipOrUrl)
{
    if (IpAddress::isIpAddress(ipOrUrl))
    {
        return removeLocation(IpAddress(ipOrUrl));
    }
    else if (Url::isUrl(ipOrUrl))
    {
        return removeLocation(Url(ipOrUrl));
    }
    else
    {
        std::cout << "Provided string is neither ip nor url\n";
    }
    return false;
}

ILocationManager::LocationStatus BasicLocationManager::getLocationStatus(std::string ipOrUrl)
{
    if (IpAddress::isIpAddress(ipOrUrl))
    {
        return getLocationStatus(IpAddress(ipOrUrl));
    }
    else if (Url::isUrl(ipOrUrl))
    {
        return getLocationStatus(Url(ipOrUrl));
    }
    else
    {
        std::cout << "Provided string is neither ip nor url\n";
    }
    return LocationStatus::WRONG_STATUS;
}

std::vector<IpLocationData> BasicLocationManager::getLocations(std::string ipOrUrl)
{
    if (IpAddress::isIpAddress(ipOrUrl))
    {
        return getLocations(IpAddress(ipOrUrl));
    }
    else if (Url::isUrl(ipOrUrl))
    {
        return getLocations(Url(ipOrUrl));
    }
    else
    {
        std::cout << "Provided string is neither ip nor url\n";
    }
    return std::vector<IpLocationData>{};
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
                    << errorString
                    << '\n';
    };

    mLocationProvider->getByIp(ipAddress, successCallback, failureCallback);
}

void BasicLocationManager::updateLocation(Url url)
{
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

bool BasicLocationManager::removeLocation(IpAddress ipAddress)
{
    return mDatabase->remove(ipAddress);
}

bool BasicLocationManager::removeLocation(Url url)
{
    return mDatabase->remove(url);
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
    IpLocationData receivedNewLocation = {geolocationData, ipAddress};

    auto urlIter = mIpToUrlMap.find(ipAddress);
    if (urlIter != mIpToUrlMap.end())
    {
        receivedNewLocation.url = urlIter->second;
        mIpToUrlMap.erase(urlIter);
    }
    mDatabase->add(receivedNewLocation);
    emit newLocation(receivedNewLocation);

    finishedSuccesfully(ipAddress);
    if (!receivedNewLocation.url.isEmpty())
    {
        finishedSuccesfully(receivedNewLocation.url);
    }
}
