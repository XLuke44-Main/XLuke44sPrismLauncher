#pragma once

#include <QByteArray>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

namespace Katabasis {

constexpr int defaultTimeout = 30 * 1000;

class Reply : public QTimer {
    Q_OBJECT

   public:
    Reply(QNetworkReply* reply, int timeOut = defaultTimeout, QObject* parent = 0);

   signals:
    void error(QNetworkReply::NetworkError);

   public slots:

    void onTimeOut();

   public:
    QNetworkReply* reply;
    bool timedOut = false;
};

class ReplyList {
   public:
    ReplyList() { ignoreSslErrors_ = false; }

    virtual ~ReplyList();

    void add(QNetworkReply* reply, int timeOut = defaultTimeout);

    void add(Reply* reply);

    void remove(QNetworkReply* reply);

    Reply* find(QNetworkReply* reply);

    bool ignoreSslErrors();
    void setIgnoreSslErrors(bool ignoreSslErrors);

   protected:
    QList<Reply*> replies_;
    bool ignoreSslErrors_;
};

}   
