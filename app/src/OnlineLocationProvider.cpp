
#include "OnlineLocationProvider.hpp"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

OnlineLocationProvider::OnlineLocationProvider(std::unique_ptr<INetworkAccessHandler> networkAccessHandler,
                                               std::string hostName,
                                               std::string accessKey)
: mHostName{hostName}
, mAccessKey{accessKey}
, mNetworkAccessHandler{std::move(networkAccessHandler)}
{
}

void OnlineLocationProvider::getByIp(IpAddress address,
                                     SuccessCallback&& successCallback,
                                     FailureCallback&& failureCallback)
{
    Url fullResuestUrl = mHostName
                         + "/"
                         +  address.toString()
                         +  "?access_key="
                         +  mAccessKey;

    mNetworkAccessHandler->get(fullResuestUrl, [this, successCallback, failureCallback](auto networkReply)
    {
        std::string failureInfo;
        auto geolocation = parseGeolocation(networkReply, failureInfo);
        if (geolocation)
        {
            successCallback(geolocation.value());
        }
        else
        {
            failureCallback(failureInfo);
        }
    });
}

std::optional<GeolocationData> OnlineLocationProvider::parseGeolocation(NetworkReply networkReply, std::string& failureInfo)
{
    failureInfo.clear();
    if (!checkNetworkReply(networkReply, failureInfo))
    {
        return std::nullopt;
    }

    auto strReply = networkReply.read();

    QJsonParseError jsonConversionError;
    QJsonDocument jsonResponse(QJsonDocument::fromJson(QString::fromStdString(strReply).toUtf8(), &jsonConversionError));
    if (jsonConversionError.error != QJsonParseError::ParseError::NoError)
    {
        failureInfo = "JSON parsing error: " + jsonConversionError.errorString().toStdString();
        return std::nullopt;
    }

    QJsonObject jsonObj = jsonResponse.object();
    auto latitudeQString = QString::fromStdString(LATITUDE);
    auto longitudeQString = QString::fromStdString(LONGITUDE);
    if (!jsonObj.contains(latitudeQString) || !jsonObj.contains(longitudeQString))
    {
        failureInfo = "JSON do not contains geolocation data";
        return std::nullopt;
    }

    return GeolocationData{jsonObj[latitudeQString].toDouble(), jsonObj[longitudeQString].toDouble()};
}

bool OnlineLocationProvider::checkNetworkReply(NetworkReply networkReply, std::string& failureInfo)
{
    failureInfo.clear();
    if (networkReply.error())
    {
        failureInfo = networkReply.getErrorString();
        return false;
    }

    const auto statusCode = networkReply.getStatusCode();
    if (statusCode != NetworkReply::OK_STATUS_CODE)
    {
        failureInfo = "Received NOK response. Status code: "
                      + std::to_string(statusCode)
                      + ".";
        return false;
    }

    return true;
}
