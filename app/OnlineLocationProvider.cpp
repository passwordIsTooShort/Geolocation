
#include "OnlineLocationProvider.hpp"

#include <iostream>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

bool OnlineLocationProvider::prepareProvider()
{
    QObject::connect(&mNetworkAccessManager, &QNetworkAccessManager::finished, [this](QNetworkReply* reply)
    {
        if (reply->error())
        {
            std::cerr << "Received error response. Error: " << reply->errorString().toStdString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
            return;
        }
        
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != OK_STATUS_CODE)
        {
            std::cerr << "Received not OK response while doing request for ip: " << getIpFromRequest(reply->request()).toStdString()
                        << ". Status code: " << statusCode << ". Reason phase attribute: "
                        << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toStdString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
        }
        

        QString strReply = QString(reply->readAll());

        QJsonParseError jsonConversionError;
        QJsonDocument jsonResponse(QJsonDocument::fromJson(strReply.toUtf8(), &jsonConversionError));
        if (jsonConversionError.error != QJsonParseError::ParseError::NoError)
        {
            std::cerr << "There is json parsing error while processing request for IP: " << getIpFromRequest(reply->request()).toStdString()
                        << ". Error: " << jsonConversionError.errorString().toStdString() << "\n";
            // TODO: Use error callback (once it will be added to interface)
        }
        

        QJsonObject jsonObj = jsonResponse.object();
        mOnNewGeolocationCallback(jsonObj[QString("ip")].toString().toStdString(),
                                    mAccessKey,
                                    GeolocationData(jsonObj[QString("latitude")].toDouble(),
                                                    jsonObj[QString("longitude")].toDouble())
                                    );
	});

    return true;
}

void OnlineLocationProvider::getByIp(IpAdress address)
{
    std::cout << "Requesting geolocation for: " << address.mIpAddress << "\n";
    QString fullResuestUrl(QString::fromStdString(mHostName));
    fullResuestUrl += "/";
    fullResuestUrl += QString::fromStdString(address.mIpAddress);
    fullResuestUrl += "?access_key=";
    fullResuestUrl += QString::fromStdString(mAccessKey);

    mNetworkAccessManager.get(QNetworkRequest(fullResuestUrl));
}

QString OnlineLocationProvider::getIpFromRequest(QNetworkRequest)
{
    // TODO:
    return "0.0.0.0";
}
