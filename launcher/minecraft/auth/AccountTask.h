// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#pragma once

#include <tasks/Task.h>

#include <qsslerror.h>
#include <QJsonObject>
#include <QString>
#include <QTimer>

#include "MinecraftAccount.h"

class QNetworkReply;

enum class AccountTaskState {
    STATE_CREATED,
    STATE_WORKING,
    STATE_SUCCEEDED,
    STATE_DISABLED,      
    STATE_FAILED_SOFT,   
    STATE_FAILED_HARD,   
    STATE_FAILED_GONE,   
    STATE_OFFLINE        
};

class AccountTask : public Task {
    Q_OBJECT
   public:
    explicit AccountTask(AccountData* data, QObject* parent = 0);
    virtual ~AccountTask(){};

    AccountTaskState m_taskState = AccountTaskState::STATE_CREATED;

    AccountTaskState taskState() { return m_taskState; }

   signals:
    void showVerificationUriAndCode(const QUrl& uri, const QString& code, int expiresIn);
    void hideVerificationUriAndCode();

   protected:

    virtual QString getStateMessage() const;

   protected slots:

    bool changeState(AccountTaskState newState, QString reason = QString());

   protected:
    AccountData* m_data = nullptr;
};
