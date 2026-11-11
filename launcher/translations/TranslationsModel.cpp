// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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

#include "TranslationsModel.h"

#include <QCoreApplication>
#include <QResource>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QVector>

#include "Application.h"

namespace {
const QString defaultLangCode = QStringLiteral("en_GB");
const QString polishLangCode = QStringLiteral("pl_PL");

struct Language {
    QString key;
    QString nativeName;
};

const QVector<Language> supportedLanguages = {
    { defaultLangCode, QStringLiteral("English (United Kingdom)") },
    { polishLangCode, QStringLiteral("Polski") },
};

bool isSupportedLanguage(const QString& key)
{
    return key == defaultLangCode || key == polishLangCode;
}

QString embeddedCatalogPath(const QString& locale)
{
    return QStringLiteral(":/translations/mmc_%1.qm").arg(locale);
}

QString currentFormattingLocale(bool useSystemLocale, const QString& selectedLanguage)
{
    return useSystemLocale ? QLocale::system().name() : selectedLanguage;
}
}   

struct TranslationsModel::Private {
    QString selectedLanguage = defaultLangCode;

    std::unique_ptr<QTranslator> englishTranslator;
    std::unique_ptr<QTranslator> qtTranslator;
    std::unique_ptr<QTranslator> selectedTranslator;
};

TranslationsModel::TranslationsModel(QObject* parent) : QAbstractListModel(parent), d(std::make_unique<Private>())
{

    Q_INIT_RESOURCE(translations);
}

TranslationsModel::~TranslationsModel()
{
    if (d->selectedTranslator) {
        QCoreApplication::removeTranslator(d->selectedTranslator.get());
    }
    if (d->qtTranslator) {
        QCoreApplication::removeTranslator(d->qtTranslator.get());
    }
    if (d->englishTranslator) {
        QCoreApplication::removeTranslator(d->englishTranslator.get());
    }
}

QVariant TranslationsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= supportedLanguages.size() || index.column() != 0) {
        return {};
    }

    const auto& language = supportedLanguages.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return language.nativeName;
        case Qt::ToolTipRole:
            return language.key;
        case Qt::UserRole:
            return language.key;
        default:
            return {};
    }
}

QVariant TranslationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && section == 0 && role == Qt::DisplayRole) {
        return tr("Language");
    }
    return QAbstractListModel::headerData(section, orientation, role);
}

int TranslationsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : supportedLanguages.size();
}

int TranslationsModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

bool TranslationsModel::selectLanguage(QString key)
{
    if (!isSupportedLanguage(key)) {
        qWarning() << "Selected unsupported language" << key << ", defaulting to" << defaultLangCode;
        key = defaultLangCode;
        APPLICATION->settings()->set("Language", key);
    }

    if (d->selectedTranslator) {
        QCoreApplication::removeTranslator(d->selectedTranslator.get());
        d->selectedTranslator.reset();
    }
    if (d->qtTranslator) {
        QCoreApplication::removeTranslator(d->qtTranslator.get());
        d->qtTranslator.reset();
    }
    if (d->englishTranslator) {
        QCoreApplication::removeTranslator(d->englishTranslator.get());
        d->englishTranslator.reset();
    }

    const bool useSystemLocale = APPLICATION->settings()->get("UseSystemLocale").toBool();
    QLocale::setDefault(QLocale(currentFormattingLocale(useSystemLocale, key)));

    d->englishTranslator = std::make_unique<QTranslator>();
    const bool englishLoaded = d->englishTranslator->load(embeddedCatalogPath(defaultLangCode));
    if (englishLoaded) {
        QCoreApplication::installTranslator(d->englishTranslator.get());
    } else {
        qCritical() << "Could not load embedded English UK translation catalog; using source strings";
        d->englishTranslator.reset();
    }

    std::unique_ptr<QTranslator> selectedTranslator;
    if (key == polishLangCode) {
        selectedTranslator = std::make_unique<QTranslator>();
        if (!selectedTranslator->load(embeddedCatalogPath(polishLangCode))) {
            qCritical() << "Could not load embedded Polish translation catalog; defaulting to" << defaultLangCode;
            selectedTranslator.reset();
            key = defaultLangCode;
            APPLICATION->settings()->set("Language", key);
            QLocale::setDefault(QLocale(currentFormattingLocale(useSystemLocale, key)));
        }
    }

    d->qtTranslator = std::make_unique<QTranslator>();
    const auto qtTranslationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    if (d->qtTranslator->load(QStringLiteral("qt_%1").arg(key), qtTranslationsPath)) {
        if (!QCoreApplication::installTranslator(d->qtTranslator.get())) {
            qWarning() << "Could not install Qt translation catalog for" << key;
            d->qtTranslator.reset();
        }
    } else {
        d->qtTranslator.reset();
    }

    bool selectedLoaded = key == defaultLangCode;
    if (selectedTranslator) {
        if (QCoreApplication::installTranslator(selectedTranslator.get())) {
            d->selectedTranslator = std::move(selectedTranslator);
            selectedLoaded = true;
        } else {
            qCritical() << "Could not install embedded Polish translation catalog; defaulting to" << defaultLangCode;
            selectedTranslator.reset();
            if (d->qtTranslator) {
                QCoreApplication::removeTranslator(d->qtTranslator.get());
                d->qtTranslator.reset();
            }
            key = defaultLangCode;
            APPLICATION->settings()->set("Language", key);
            QLocale::setDefault(QLocale(currentFormattingLocale(useSystemLocale, key)));

            d->qtTranslator = std::make_unique<QTranslator>();
            if (d->qtTranslator->load(QStringLiteral("qt_%1").arg(key), qtTranslationsPath)) {
                if (!QCoreApplication::installTranslator(d->qtTranslator.get())) {
                    d->qtTranslator.reset();
                }
            } else {
                d->qtTranslator.reset();
            }
        }
    }

    d->selectedLanguage = key;
    return selectedLoaded || key == defaultLangCode;
}

QModelIndex TranslationsModel::selectedIndex() const
{
    for (int row = 0; row < supportedLanguages.size(); ++row) {
        if (supportedLanguages.at(row).key == d->selectedLanguage) {
            return index(row, 0);
        }
    }
    return index(0, 0);
}

QString TranslationsModel::selectedLanguage() const
{
    return d->selectedLanguage;
}

void TranslationsModel::setUseSystemLocale(bool useSystemLocale)
{
    APPLICATION->settings()->set("UseSystemLocale", useSystemLocale);
    QLocale::setDefault(QLocale(currentFormattingLocale(useSystemLocale, d->selectedLanguage)));
}
