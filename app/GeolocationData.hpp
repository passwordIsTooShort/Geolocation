#ifndef APP_GEOLOCATION_DATA_HPP_
#define APP_GEOLOCATION_DATA_HPP_

class GeolocationData
{
public:
    GeolocationData(double latitude, double longitude)
    : mLatitude(latitude)
    , mLongitude(longitude)
    {}
    
    ~GeolocationData() = default;

    double getLatitude() const
    {
        return mLatitude;
    }

    double geLongitude() const
    {
        return mLongitude;
    }

private:
    double mLatitude;
    double mLongitude;
};

#endif // APP_GEOLOCATION_DATA_HPP_