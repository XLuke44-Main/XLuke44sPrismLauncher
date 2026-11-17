// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
 *  Copyright (C) 2023 TheKodeToad <TheKodeToad@proton.me>
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
#include <cassert>

#include <QDateTime>
#include <QMenu>
#include <QObject>
#include <QProcess>
#include <QSet>
#include "QObjectPtr.h"

#include "settings/SettingsObject.h"

#include "BaseVersionList.h"
#include "MessageLevel.h"
#include "minecraft/auth/MinecraftAccount.h"
#include "pathmatcher/IPathMatcher.h"
#include "settings/INIFile.h"

#include "net/Mode.h"

#include "RuntimeContext.h"
#include "minecraft/launch/MinecraftServerTarget.h"

class QDir;
class Task;
class LaunchTask;
class BaseInstance;

using InstancePtr = std::shared_ptr<BaseInstance>;

class BaseInstance : public QObject, public std::enable_shared_from_this<BaseInstance> {
    Q_OBJECT
   protected:

    BaseInstance(SettingsObjectPtr globalSettings, SettingsObjectPtr settings, const QString& rootDir);

   public:  
    enum class Status {
        Present,
        Gone   
    };

   public:

    virtual ~BaseInstance() {}

    virtual void saveNow() = 0;

    void invalidate();

    virtual QString id() const;

    void setRunning(bool running);
    bool isRunning() const;
    int64_t totalTimePlayed() const;
    int64_t lastTimePlayed() const;
    void resetTimePlayed();

    QString instanceType() const;

    QString instanceRoot() const;

    virtual QString gameRoot() const { return instanceRoot(); }

    virtual QString modsRoot() const = 0;

    QString name() const;
    void setName(QString val);

    QString windowTitle() const;

    QString iconKey() const;
    void setIconKey(QString val);

    QString notes() const;
    void setNotes(QString val);

    QString getPreLaunchCommand();
    QString getPostExitCommand();
    QString getWrapperCommand();

    bool isManagedPack() const;
    QString getManagedPackType() const;
    QString getManagedPackID() const;
    QString getManagedPackName() const;
    QString getManagedPackVersionID() const;
    QString getManagedPackVersionName() const;
    void setManagedPack(const QString& type, const QString& id, const QString& name, const QString& versionId, const QString& version);
    void copyManagedPack(BaseInstance& other);

    virtual MessageLevel::Enum guessLevel([[maybe_unused]] const QString& line, MessageLevel::Enum level) { return level; }

    virtual QStringList extraArguments();

    virtual QSet<QString> traits() const = 0;

    qint64 lastLaunch() const;

    void setLastLaunch(qint64 val = QDateTime::currentMSecsSinceEpoch());

    virtual SettingsObjectPtr settings();

    virtual void loadSpecificSettings() = 0;

    virtual Task::Ptr createUpdateTask(Net::Mode mode) = 0;

    virtual shared_qobject_ptr<LaunchTask> createLaunchTask(AuthSessionPtr account, MinecraftServerTargetPtr serverToJoin) = 0;

    shared_qobject_ptr<LaunchTask> getLaunchTask();

    virtual QProcessEnvironment createEnvironment() = 0;
    virtual QProcessEnvironment createLaunchEnvironment() = 0;

    virtual IPathMatcher::Ptr getLogFileMatcher() = 0;

    virtual QString getLogFileRoot() = 0;

    virtual QString getStatusbarDescription() = 0;

    virtual QString instanceConfigFolder() const = 0;

    virtual QMap<QString, QString> getVariables() = 0;

    virtual QString typeName() const = 0;

    void updateRuntimeContext();
    RuntimeContext runtimeContext() const { return m_runtimeContext; }

    bool hasVersionBroken() const { return m_hasBrokenVersion; }
    void setVersionBroken(bool value)
    {
        if (m_hasBrokenVersion != value) {
            m_hasBrokenVersion = value;
            emit propertiesChanged(this);
        }
    }

    bool hasUpdateAvailable() const { return m_hasUpdate; }
    void setUpdateAvailable(bool value)
    {
        if (m_hasUpdate != value) {
            m_hasUpdate = value;
            emit propertiesChanged(this);
        }
    }

    bool hasCrashed() const { return m_crashed; }
    void setCrashed(bool value)
    {
        if (m_crashed != value) {
            m_crashed = value;
            emit propertiesChanged(this);
        }
    }

    virtual bool canLaunch() const;
    virtual bool canEdit() const = 0;
    virtual bool canExport() const = 0;

    virtual void populateLaunchMenu(QMenu* menu) = 0;

    bool reloadSettings();

    virtual QStringList verboseDescription(AuthSessionPtr session, MinecraftServerTargetPtr serverToJoin) = 0;

    Status currentStatus() const;

    int getConsoleMaxLines() const;
    bool shouldStopOnConsoleOverflow() const;

    QStringList getLinkedInstances() const;
    void setLinkedInstances(const QStringList& list);
    void addLinkedInstanceId(const QString& id);
    bool removeLinkedInstanceId(const QString& id);
    bool isLinkedToInstanceId(const QString& id) const;

   protected:
    void changeStatus(Status newStatus);

    SettingsObjectPtr globalSettings() const { return m_global_settings.lock(); }

    bool isSpecificSettingsLoaded() const { return m_specific_settings_loaded; }
    void setSpecificSettingsLoaded(bool loaded) { m_specific_settings_loaded = loaded; }

   signals:

    void propertiesChanged(BaseInstance* inst);

    void launchTaskChanged(shared_qobject_ptr<LaunchTask>);

    void runningStatusChanged(bool running);

    void profilerChanged();

    void statusChanged(Status from, Status to);

   protected slots:
    void iconUpdated(QString key);

   protected:  
    QString m_rootDir;
    SettingsObjectPtr m_settings;

    bool m_isRunning = false;
    shared_qobject_ptr<LaunchTask> m_launchProcess;
    QDateTime m_timeStarted;
    RuntimeContext m_runtimeContext;

   private:  
    Status m_status = Status::Present;
    bool m_crashed = false;
    bool m_hasUpdate = false;
    bool m_hasBrokenVersion = false;

    SettingsObjectWeakPtr m_global_settings;
    bool m_specific_settings_loaded = false;
};

Q_DECLARE_METATYPE(shared_qobject_ptr<BaseInstance>)

