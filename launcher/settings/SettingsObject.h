/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>
#include <memory>

class Setting;
class SettingsObject;

using SettingsObjectPtr = std::shared_ptr<SettingsObject>;
using SettingsObjectWeakPtr = std::weak_ptr<SettingsObject>;

class SettingsObject : public QObject {
    Q_OBJECT
   public:
    class Lock {
       public:
        Lock(SettingsObjectPtr locked) : m_locked(locked) { m_locked->suspendSave(); }
        ~Lock() { m_locked->resumeSave(); }

       private:
        SettingsObjectPtr m_locked;
    };

   public:
    explicit SettingsObject(QObject* parent = 0);
    virtual ~SettingsObject();

    std::shared_ptr<Setting> registerOverride(std::shared_ptr<Setting> original, std::shared_ptr<Setting> gate);

    std::shared_ptr<Setting> registerPassthrough(std::shared_ptr<Setting> original, std::shared_ptr<Setting> gate);

    std::shared_ptr<Setting> registerSetting(QStringList synonyms, QVariant defVal = QVariant());

    std::shared_ptr<Setting> registerSetting(QString id, QVariant defVal = QVariant()) { return registerSetting(QStringList(id), defVal); }

    std::shared_ptr<Setting> getSetting(const QString& id) const;

    QVariant get(const QString& id) const;

    bool set(const QString& id, QVariant value);

    void reset(const QString& id) const;

    bool contains(const QString& id);

    virtual bool reload();

    virtual void suspendSave() = 0;
    virtual void resumeSave() = 0;
   signals:

    void SettingChanged(const Setting& setting, QVariant value);

    void settingReset(const Setting& setting);

   protected slots:

    virtual void changeSetting(const Setting& setting, QVariant value) = 0;

    virtual void resetSetting(const Setting& setting) = 0;

   protected:

    void connectSignals(const Setting& setting);

    virtual QVariant retrieveValue(const Setting& setting) = 0;

    friend class Setting;

   private:
    QMap<QString, std::shared_ptr<Setting>> m_settings;

   protected:
    bool m_suspendSave = false;
    bool m_doSave = false;
};
