#pragma once

#include <QByteArray>
#include <QMap>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QTimer>

class QNetworkAccessManager;

namespace Katabasis {

class PollServer : public QObject {
    Q_OBJECT

   public:
    explicit PollServer(QNetworkAccessManager* manager,
                        const QNetworkRequest& request,
                        const QByteArray& payload,
                        int expiresIn,
                        QObject* parent = 0);

    Q_PROPERTY(int interval READ interval WRITE setInterval)
    int interval() const;
    void setInterval(int interval);

   signals:
    void verificationReceived(QMap<QString, QString>);
    void serverClosed(bool);   

   public slots:
    void startPolling();

   protected slots:
    void onPollTimeout();
    void onExpiration();
    void onReplyFinished();

   protected:
    QNetworkAccessManager* manager_;
    const QNetworkRequest request_;
    const QByteArray payload_;
    const int expiresIn_;
    QTimer expirationTimer;
    QTimer pollTimer;
};

}   
