#include "NetworkAccessHandler.hpp"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

void NetworkAccessHandler::get(const Url& request, ResponseCallback&& responseCallback)
{
    QNetworkRequest qtNetworkRequest(QString::fromStdString(request.toString()));
    auto qtNetworkresponse = mNetworkAccessManager.get(qtNetworkRequest);

    QObject::connect(qtNetworkresponse, &QNetworkReply::finished, [qtNetworkresponse, responseCallback]()
    {
        auto errorString = std::string("");
        if (qtNetworkresponse->error())
        {
            errorString = qtNetworkresponse->errorString().toStdString();
        }
        auto statusCode = qtNetworkresponse->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        auto messageContent = QString(qtNetworkresponse->readAll()).toStdString();
        NetworkReply reply(messageContent, statusCode, errorString);

        responseCallback(reply);

        qtNetworkresponse->deleteLater();
    });
}
