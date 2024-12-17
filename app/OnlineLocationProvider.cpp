
#include "OnlineLocationProvider.hpp"

#include <iostream>
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
    QObject::connect(&mNetworkAccessManager, &QNetworkAccessManager::finished, [this](QNetworkReply* reply)
    {
        qInfo() << "Ip from request: " << getIpFromRequest(reply->request()) << "\n";

        if (reply->error())
        {
            qWarning() << "Received error response. Error: " << reply->errorString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
            return;
        }
        
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != OK_STATUS_CODE)
        {
            qWarning() << "Received not OK response while doing request for ip: " << getIpFromRequest(reply->request())
                        << ". Status code: " << statusCode << ". Reason phrase attribute: "
                        << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
            return;
        }

        QString strReply = QString(reply->readAll());
        QJsonParseError jsonConversionError;
        QJsonDocument jsonResponse(QJsonDocument::fromJson(strReply.toUtf8(), &jsonConversionError));
        if (jsonConversionError.error != QJsonParseError::ParseError::NoError)
        {
            qWarning() << "There is json parsing error while processing request for IP: " << getIpFromRequest(reply->request())
                        << ". Error: " << jsonConversionError.errorString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
            return;
        }

        if (!mOnNewGeolocationCallback)
        {
            qWarning() << "Response for IP: " << getIpFromRequest(reply->request()) << " received and processed correctly"
                       << ", but callback to handle location is not provided. Dropping this request";
            // TODO: Instead of drop - just add to the queue. As soon as callback will be ready,
            // it can be called for all previous requests
        }
        QJsonObject jsonObj = jsonResponse.object();
        mOnNewGeolocationCallback(jsonObj[QString("ip")].toString().toStdString(),
                                    mAccessKey.toStdString(),
                                    GeolocationData(jsonObj[QString("latitude")].toDouble(),
                                                    jsonObj[QString("longitude")].toDouble())
                                    );
	});
}

void OnlineLocationProvider::getByIp(IpAdress address)
{
    if (!isReady())
    {
        // TODO: Remove when queue functionality will be done. Stay for now, to not "waste" requests on API
        qWarning() << "Request cannot be done - Location provider not ready for handling request\n";
        return;
    }

    qDebug() << "Requesting geolocation for: " << QString::fromStdString(address.getIpAddress()) << "\n";
    QString fullResuestUrl = mHostName
                             %"/"
                             % QString::fromStdString(address.getIpAddress())
                             % "?access_key="
                             % mAccessKey;

    mNetworkAccessManager.get(QNetworkRequest(fullResuestUrl));
}

bool OnlineLocationProvider::isReady() const
{
    return mOnNewGeolocationCallback != nullptr;
}

QString OnlineLocationProvider::getIpFromRequest(QNetworkRequest request)
{
    QRegularExpression ipFromRequestRegularExpression("^http.*\\/(\\d+\\.\\d+\\.\\d+\\.\\d+)\\?.*");
    QRegularExpressionMatch ipMatch = ipFromRequestRegularExpression.match(request.url().toString());
    if (!ipMatch.hasMatch())
    {
        qWarning() << "Cannot parse ip from request with url: " << request.url().toString() << "\n";
        return "";
    }

    return ipMatch.captured(1);
}
