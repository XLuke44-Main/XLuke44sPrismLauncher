#pragma once
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>

#include "katabasis/Reply.h"

class AuthRequest : public QObject {
    Q_OBJECT

   public:
    explicit AuthRequest(QObject* parent = 0);
    ~AuthRequest();

   public slots:
    void get(const QNetworkRequest& req, int timeout = 60 * 1000);
    void post(const QNetworkRequest& req, const QByteArray& data, int timeout = 60 * 1000);

   signals:

    void finished(QNetworkReply::NetworkError error, QByteArray data, QList<QNetworkReply::RawHeaderPair> headers);

    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

   protected slots:

    void onRequestFinished();

    void onRequestError(QNetworkReply::NetworkError error);

    void onSslErrors(QList<QSslError> errors);

    void finish();

    void onUploadProgress(qint64 uploaded, qint64 total);

   public:
    QNetworkReply::NetworkError error_;
    int httpStatus_ = 0;
    QString errorString_;

   protected:
    void setup(const QNetworkRequest& request, QNetworkAccessManager::Operation operation, const QByteArray& verb = QByteArray());

    enum Status { Idle, Requesting, ReRequesting };

    QNetworkRequest request_;
    QByteArray data_;
    QNetworkReply* reply_;
    Status status_;
    QNetworkAccessManager::Operation operation_;
    QUrl url_;
    Katabasis::ReplyList timedReplies_;

    QTimer* timer_;
};
