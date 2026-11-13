#pragma once

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVariantMap>

namespace Katabasis {
enum class Activity {
    Idle,
    LoggingIn,
    LoggingOut,
    Refreshing,
    FailedSoft,   
    FailedHard,   
    FailedGone,   
    Succeeded
};

enum class Validity { None, Assumed, Certain };

struct Token {
    QDateTime issueInstant;
    QDateTime notAfter;
    QString token;
    QString refresh_token;
    QVariantMap extra;

    Validity validity = Validity::None;
    bool persistent = true;
};

}   
