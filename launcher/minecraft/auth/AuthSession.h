#pragma once

#include <QMultiMap>
#include <QString>
#include <memory>
#include "QObjectPtr.h"

class MinecraftAccount;
class QNetworkAccessManager;

struct AuthSession {
    bool MakeOffline(QString offline_playername);
    void MakeDemo();

    QString serializeUserProperties();

    enum Status {
        Undetermined,
        RequiresOAuth,
        RequiresPassword,
        RequiresProfileSetup,
        PlayableOffline,
        PlayableOnline,
        GoneOrMigrated
    } status = Undetermined;

    QString session;

    QString access_token;

    QString player_name;

    QString uuid;

    QString user_type;

    bool auth_server_online = false;

    bool wants_online = true;

    bool demo = false;
};

using AuthSessionPtr = std::shared_ptr<AuthSession>;
