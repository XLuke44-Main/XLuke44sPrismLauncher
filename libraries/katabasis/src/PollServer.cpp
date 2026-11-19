#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "JsonResponse.h"
#include "katabasis/PollServer.h"

namespace {
QMap<QString, QString> toVerificationParams(const QVariantMap& map)
{
    QMap<QString, QString> params;
    for (QVariantMap::const_iterator i = map.constBegin(); i != map.constEnd(); ++i) {
        params[i.key()] = i.value().toString();
    }
    return params;
}
}   

namespace Katabasis {

PollServer::PollServer(QNetworkAccessManager* manager,
                       const QNetworkRequest& request,
                       const QByteArray& payload,
                       int expiresIn,
                       QObject* parent)
    : QObject(parent), manager_(manager), request_(request), payload_(payload), expiresIn_(expiresIn)
{
    expirationTimer.setTimerType(Qt::VeryCoarseTimer);
    expirationTimer.setInterval(expiresIn * 1000);
    expirationTimer.setSingleShot(true);
    connect(&expirationTimer, SIGNAL(timeout()), this, SLOT(onExpiration()));
    expirationTimer.start();

    pollTimer.setTimerType(Qt::VeryCoarseTimer);
    pollTimer.setInterval(5 * 1000);
    pollTimer.setSingleShot(true);
    connect(&pollTimer, SIGNAL(timeout()), this, SLOT(onPollTimeout()));
}

int PollServer::interval() const
{
    return pollTimer.interval() / 1000;
}

void PollServer::setInterval(int interval)
{
    pollTimer.setInterval(interval * 1000);
}

void PollServer::startPolling()
{
    if (expirationTimer.isActive()) {
        pollTimer.start();
    }
}

void PollServer::onPollTimeout()
{
    qDebug() << "PollServer::onPollTimeout: retrying";
    QNetworkReply* reply = manager_->post(request_, payload_);
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void PollServer::onExpiration()
{
    pollTimer.stop();
    emit serverClosed(false);
}

void PollServer::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        qDebug() << "PollServer::onReplyFinished: reply is null";
        return;
    }

    QByteArray replyData = reply->readAll();
    QMap<QString, QString> params = toVerificationParams(parseJsonResponse(replyData));

    if (reply->error() == QNetworkReply::TimeoutError) {

        setInterval(interval() * 2);
        pollTimer.start();
    } else {
        QString error = params.value("error");
        if (error == "slow_down") {

            setInterval(interval() + 5);
            pollTimer.start();
        } else if (error == "authorization_pending") {

            pollTimer.start();
        } else {
            expirationTimer.stop();
            emit serverClosed(true);

            emit verificationReceived(params);
        }
    }
    reply->deleteLater();
}

}   
