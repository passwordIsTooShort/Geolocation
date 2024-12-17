#include "SqlDatabase.hpp"

bool SqlDatabase::connectToDatabase()
{
    return true;
}

bool SqlDatabase::add(GeolocationData locationData, IpAdress address)
{
    return true;
}

bool SqlDatabase::removeByIpAdress(IpAdress address)
{
    return true;
}

bool SqlDatabase::removeByGeolocation(GeolocationData locationData)
{
    return true;
}

GeolocationData SqlDatabase::getLocation(IpAdress address) const
{
    return GeolocationData(-1, -1);
}
