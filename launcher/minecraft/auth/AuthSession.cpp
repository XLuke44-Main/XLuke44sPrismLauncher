#include "AuthSession.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

QString AuthSession::serializeUserProperties()
{
    QJsonObject userAttrs;

    QJsonDocument value(userAttrs);
    return value.toJson(QJsonDocument::Compact);
}

bool AuthSession::MakeOffline(QString offline_playername)
{
    if (status != PlayableOffline && status != PlayableOnline) {
        return false;
    }
    session = "-";
    access_token = "0";
    player_name = offline_playername;
    status = PlayableOffline;
    return true;
}

void AuthSession::MakeDemo()
{
    player_name = "Player";
    demo = true;
}
