#pragma once

#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPair>

#include "Bits.h"
#include "Reply.h"
#include "RequestParameter.h"

namespace Katabasis {

class ReplyServer;
class PollServer;

class DeviceFlow : public QObject {
    Q_OBJECT
   public:
    Q_ENUMS(GrantFlow)

   public:
    struct Options {
        QString userAgent = QStringLiteral("Katabasis/1.0");
        QString responseType = QStringLiteral("code");
        QString scope;
        QString clientIdentifier;
        QString clientSecret;
        QUrl authorizationUrl;
        QUrl accessTokenUrl;
    };

   public:

    bool linked();

    QString token();

    QVariantMap extraTokens();

   public:

    QVariantMap extraRequestParams();
    void setExtraRequestParams(const QVariantMap& value);

    QString grantType();
    void setGrantType(const QString& value);

   public:

    explicit DeviceFlow(Options& opts, Token& token, QObject* parent = 0, QNetworkAccessManager* manager = 0);

    QString refreshToken();

    QDateTime expires();

   public slots:

    void login();

    void logout();

    bool refresh();

    void serverHasClosed(bool paramsfound = false);

   signals:

    void openBrowser(const QUrl& url);

    void closeBrowser();

    void showVerificationUriAndCode(const QUrl& uri, const QString& code, int expiresIn);

    void activityChanged(Activity activity);

   public slots:

    void onVerificationReceived(QMap<QString, QString>);

   protected slots:

    void onDeviceAuthReplyFinished();

    void onRefreshFinished();

    void onRefreshError(QNetworkReply::NetworkError error, QNetworkReply* reply);

   protected:

    void setRefreshToken(const QString& v);

    void setExpires(QDateTime v);

    void startPollServer(const QVariantMap& params, int expiresIn);

    void setToken(const QString& v);

    void setLinked(bool v);

    void setExtraTokens(QVariantMap extraTokens);

    void setPollServer(PollServer* server);

    PollServer* pollServer() const;

    void updateActivity(Activity activity);

   protected:
    Options options_;

    QVariantMap extraReqParams_;
    QNetworkAccessManager* manager_ = nullptr;
    ReplyList timedReplies_;
    QString grantType_;

   protected:
    Token& token_;

   private:
    PollServer* pollServer_ = nullptr;
    Activity activity_ = Activity::Idle;
};

}   
