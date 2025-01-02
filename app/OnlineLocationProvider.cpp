
#include "OnlineLocationProvider.hpp"

#include <iostream>
#include <sstream>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStringBuilder>
#include <QtCore/QRegularExpression>

OnlineLocationProvider::OnlineLocationProvider(std::string hostName, std::string accessKey)
: mHostName{QString::fromStdString(hostName)}
, mAccessKey{QString::fromStdString(accessKey)}
{
}

void OnlineLocationProvider::getByIp(IpAddress address,
                                     SuccessCallback&& successCallback,
                                     FailureCallback&& failureCallback)
{
    qDebug() << "Request for geolocation for: " << QString::fromStdString(address.toString());
    QString fullResuestUrl = mHostName
                             %"/"
                             % QString::fromStdString(address.toString())
                             % "?access_key="
                             % mAccessKey;

    auto reply = mNetworkAccessManager.get(QNetworkRequest(fullResuestUrl));
    QObject::connect(reply, &QNetworkReply::finished, [this, successCallback, failureCallback, reply]()
    {
        std::string failureInfo;
        auto geolocation = parseGeolocationFromReply(reply, failureInfo);
        if (geolocation)
        {
            successCallback(geolocation.value());
        }
        else
        {
            failureCallback(failureInfo);
        }

        reply->deleteLater();
    });
}

std::optional<GeolocationData> OnlineLocationProvider::parseGeolocationFromReply(QNetworkReply* reply, std::string& failureInfo)
{
    failureInfo.clear();
    if (!checkNetworkReply(reply, failureInfo))
    {
        return std::nullopt;
    }

    QString strReply = QString(reply->readAll());
    QJsonParseError jsonConversionError;
    QJsonDocument jsonResponse(QJsonDocument::fromJson(strReply.toUtf8(), &jsonConversionError));
    if (jsonConversionError.error != QJsonParseError::ParseError::NoError)
    {
        failureInfo = "JSON parsing error: " + jsonConversionError.errorString().toStdString();
        return std::nullopt;
    }

    QJsonObject jsonObj = jsonResponse.object();
    return GeolocationData{jsonObj[QString("latitude")].toDouble(), jsonObj[QString("longitude")].toDouble()};
}

bool OnlineLocationProvider::checkNetworkReply(QNetworkReply* reply, std::string& failureInfo)
{
    failureInfo.clear();
    if (reply == nullptr)
    {
        failureInfo = "Nullptr reply";
        return false;
    }

    if (reply->error())
    {
        failureInfo = reply->errorString().toStdString();
        return false;
    }

    const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != OK_STATUS_CODE)
    {
        failureInfo = "Received NOK response. Status code: "
                      + std::to_string(statusCode)
                      + ". Reason phrase attribute: "
                      + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toStdString();
        return false;
    }

    return true;
}
