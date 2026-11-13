// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "Application.h"
#include "BuildConfig.h"
#include "net/HeaderProxy.h"

namespace Net {

class ApiHeaderProxy : public HeaderProxy {
   public:
    ApiHeaderProxy() : HeaderProxy() {}
    virtual ~ApiHeaderProxy() = default;

    static bool isSecureHost(const QUrl& url, const QString& expectedHost)
    {
        return url.scheme().compare(QStringLiteral("https"), Qt::CaseInsensitive) == 0
            && url.host().compare(expectedHost, Qt::CaseInsensitive) == 0 && url.port(443) == 443;
    }

    static bool isCurseForgeApiUrl(const QUrl& url)
    {
        return isSecureHost(url, QUrl(BuildConfig.FLAME_BASE_URL).host());
    }

    static bool isCurseForgeCdnUrl(const QUrl& url)
    {
        return isSecureHost(url, QStringLiteral("edge.forgecdn.net"));
    }

    static bool isCurseForgeUrl(const QUrl& url)
    {
        return isCurseForgeApiUrl(url) || isCurseForgeCdnUrl(url);
    }

    static QList<HeaderPair> curseForgeHeaders(const QNetworkRequest& request, const QString& apiKey)
    {
        QList<HeaderPair> headers;
        if (!apiKey.isEmpty() && apiKey != QStringLiteral("KEYPLACEHOLDER") && isCurseForgeUrl(request.url())) {
            headers.append({ "x-api-key", apiKey.toUtf8() });
        }
        return headers;
    }

   public:
    virtual QList<HeaderPair> headers(const QNetworkRequest& request) const override
    {
        QList<HeaderPair> hdrs;
        if (APPLICATION->capabilities() & Application::SupportsFlame) {
            hdrs.append(curseForgeHeaders(request, APPLICATION->getFlameAPIKey()));
        }

        if (request.url().host() == QUrl(BuildConfig.MODRINTH_PROD_URL).host() ||
            request.url().host() == QUrl(BuildConfig.MODRINTH_STAGING_URL).host()) {
            QString token = APPLICATION->getModrinthAPIToken();
            if (!token.isNull())
                hdrs.append({ "Authorization", token.toUtf8() });
        }
        return hdrs;
    };
};

}   
