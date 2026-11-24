// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <cmath>
#include <limits>
#include <utility>

#include "AssetsUtils.h"
#include "BuildConfig.h"
#include "FileSystem.h"
#include "net/ApiDownload.h"
#include "net/ChecksumValidator.h"
#include "net/Download.h"

#include "Application.h"

namespace {
QSet<QString> collectPathsFromDir(QString dirPath)
{
    QFileInfo dirInfo(dirPath);

    if (!dirInfo.exists()) {
        return {};
    }

    QSet<QString> out;

    QDirIterator iter(dirPath, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QString value = iter.next();
        QFileInfo info(value);
        if (info.isFile()) {
            out.insert(value);
            qDebug() << value;
        }
    }
    return out;
}

bool isSha1(const QString& hash)
{
    if (hash.size() != 40)
        return false;
    for (const QChar character : hash) {
        if (!((character >= QLatin1Char('0') && character <= QLatin1Char('9')) ||
              (character >= QLatin1Char('a') && character <= QLatin1Char('f')) ||
              (character >= QLatin1Char('A') && character <= QLatin1Char('F'))))
            return false;
    }
    return true;
}

bool fileMatchesAsset(const QString& path, const AssetObject& asset)
{
    const QFileInfo info(path);
    if (!info.isFile() || info.size() != asset.size)
        return false;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QCryptographicHash hash(QCryptographicHash::Sha1);
    while (!file.atEnd()) {
        const QByteArray data = file.read(1024 * 1024);
        if (data.isEmpty() && file.error() != QFileDevice::NoError)
            return false;
        hash.addData(data);
    }
    return hash.result().toHex().compare(asset.hash.toLatin1(), Qt::CaseInsensitive) == 0;
}
}   

namespace AssetsUtils {

bool loadAssetsIndexJson(const QString& assetsId, const QString& path, AssetsIndex& index, QString* error)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        const QString reason = QObject::tr("Failed to read assets index file %1: %2").arg(path, file.errorString());
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    const QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        const QString reason = parseError.error != QJsonParseError::NoError
                                   ? QObject::tr("Invalid assets index JSON: %1 at offset %2")
                                         .arg(parseError.errorString())
                                         .arg(parseError.offset)
                                   : QObject::tr("Invalid assets index JSON: root must be an object.");
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    const QJsonObject root = jsonDoc.object();
    const QJsonValue objectsValue = root.value("objects");
    if (!objectsValue.isObject()) {
        const QString reason = QObject::tr("Invalid assets index JSON: objects must be an object.");
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    AssetsIndex loadedIndex;
    loadedIndex.id = assetsId;
    const QJsonValue isVirtual = root.value("virtual");
    if (!isVirtual.isUndefined() && !isVirtual.isBool()) {
        const QString reason = QObject::tr("Invalid assets index JSON: virtual must be a boolean.");
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }
    if (!isVirtual.isUndefined())
        loadedIndex.isVirtual = isVirtual.toBool();

    const QJsonValue mapToResources = root.value("map_to_resources");
    if (!mapToResources.isUndefined() && !mapToResources.isBool()) {
        const QString reason = QObject::tr("Invalid assets index JSON: map_to_resources must be a boolean.");
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }
    if (!mapToResources.isUndefined())
        loadedIndex.mapToResources = mapToResources.toBool();

    const QJsonObject objects = objectsValue.toObject();
    for (auto iter = objects.constBegin(); iter != objects.constEnd(); ++iter) {
        if (!iter.value().isObject()) {
            const QString reason = QObject::tr("Invalid asset entry '%1' in index '%2'.").arg(iter.key(), assetsId);
            qCritical() << reason;
            if (error)
                *error = reason;
            return false;
        }

        const QJsonObject objectJson = iter.value().toObject();
        const QJsonValue hashValue = objectJson.value("hash");
        const QJsonValue sizeValue = objectJson.value("size");
        const double assetSize = sizeValue.toDouble(-1);
        if (!hashValue.isString() || !isSha1(hashValue.toString()) || !sizeValue.isDouble() || !std::isfinite(assetSize) ||
            assetSize < 0 || std::floor(assetSize) != assetSize ||
            static_cast<long double>(assetSize) > static_cast<long double>(std::numeric_limits<qint64>::max())) {
            const QString reason = QObject::tr("Invalid hash or size for asset '%1' in index '%2'.").arg(iter.key(), assetsId);
            qCritical() << reason;
            if (error)
                *error = reason;
            return false;
        }

        AssetObject object;
        object.hash = hashValue.toString().toLower();
        object.size = static_cast<qint64>(assetSize);
        loadedIndex.objects.insert(iter.key(), object);
    }

    index = std::move(loadedIndex);
    if (error)
        error->clear();
    return true;
}

QDir getAssetsDir(const QString& assetsId, const QString& resourcesFolder)
{
    QDir assetsDir = QDir("assets/");
    QDir indexDir = QDir(FS::PathCombine(assetsDir.path(), "indexes"));
    QDir objectDir = QDir(FS::PathCombine(assetsDir.path(), "objects"));
    QDir virtualDir = QDir(FS::PathCombine(assetsDir.path(), "virtual"));

    QString indexPath = FS::PathCombine(indexDir.path(), assetsId + ".json");
    QFile indexFile(indexPath);
    QDir virtualRoot(FS::PathCombine(virtualDir.path(), assetsId));

    if (!indexFile.exists()) {
        qCritical() << "No assets index file" << indexPath << "; can't determine assets path!";
        return virtualRoot;
    }

    AssetsIndex index;
    if (!AssetsUtils::loadAssetsIndexJson(assetsId, indexPath, index)) {
        qCritical() << "Failed to load asset index file" << indexPath << "; can't determine assets path!";
        return virtualRoot;
    }

    QString targetPath;
    if (index.isVirtual) {
        return virtualRoot;
    } else if (index.mapToResources) {
        return QDir(resourcesFolder);
    }
    return virtualRoot;
}

bool reconstructAssets(QString assetsId, QString resourcesFolder, QString* error)
{
    const QDir assetsDir("assets/");
    const QDir indexDir(FS::PathCombine(assetsDir.path(), "indexes"));
    const QDir objectDir(FS::PathCombine(assetsDir.path(), "objects"));
    const QDir virtualDir(FS::PathCombine(assetsDir.path(), "virtual"));

    const QString indexPath = FS::PathCombine(indexDir.path(), assetsId + ".json");
    const QFileInfo indexFile(indexPath);
    const QDir virtualRoot(FS::PathCombine(virtualDir.path(), assetsId));

    if (!indexFile.isFile()) {
        const QString reason = QObject::tr("Minecraft asset index '%1' is missing at %2.").arg(assetsId, indexPath);
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    qDebug() << "reconstructAssets" << assetsDir.path() << indexDir.path() << objectDir.path() << virtualDir.path() << virtualRoot.path();

    AssetsIndex index;
    QString indexError;
    if (!AssetsUtils::loadAssetsIndexJson(assetsId, indexPath, index, &indexError)) {
        const QString reason = QObject::tr("Minecraft asset index '%1' is corrupt or unreadable: %2").arg(assetsId, indexError);
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    QString targetPath;
    bool removeLeftovers = false;
    if (index.isVirtual) {
        targetPath = virtualRoot.path();
        removeLeftovers = true;
        qDebug() << "Reconstructing virtual assets folder at" << targetPath;
    } else if (index.mapToResources) {
        targetPath = resourcesFolder;
        qDebug() << "Reconstructing resources folder at" << targetPath;
    }

    auto presentFiles = targetPath.isNull() ? QSet<QString>() : collectPathsFromDir(targetPath);
    QStringList missingObjects;
    QStringList failedCopies;
    for (const auto& map : index.objects.keys()) {
        const auto assetObject = index.objects.value(map);
        const auto originalPath = FS::PathCombine(objectDir.path(), assetObject.hash.left(2), assetObject.hash);

        if (targetPath.isNull()) {
            if (!fileMatchesAsset(originalPath, assetObject))
                missingObjects.append(originalPath);
            continue;
        }

        const auto targetPathForAsset = FS::PathCombine(targetPath, map);
        presentFiles.remove(targetPathForAsset);
        if (fileMatchesAsset(targetPathForAsset, assetObject))
            continue;

        if (!fileMatchesAsset(originalPath, assetObject)) {
            missingObjects.append(originalPath);
            continue;
        }

        const QFileInfo targetInfo(targetPathForAsset);
        if (!FS::ensureFolderPathExists(targetInfo.dir().path())) {
            failedCopies.append(targetPathForAsset);
            continue;
        }
        if (targetInfo.exists() && !QFile::remove(targetPathForAsset)) {
            failedCopies.append(targetPathForAsset);
            continue;
        }

        QFile original(originalPath);
        if (!original.copy(targetPathForAsset) || !fileMatchesAsset(targetPathForAsset, assetObject))
            failedCopies.append(targetPathForAsset);
    }

    if (!missingObjects.isEmpty()) {
        const QString reason = QObject::tr("Required Minecraft asset objects are missing or corrupt: %1")
                                   .arg(missingObjects.join(QStringLiteral(", ")));
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    if (!failedCopies.isEmpty()) {
        const QString reason = QObject::tr("Failed to restore Minecraft asset files: %1")
                                   .arg(failedCopies.join(QStringLiteral(", ")));
        qCritical() << reason;
        if (error)
            *error = reason;
        return false;
    }

    if (removeLeftovers) {
        for (const auto& file : presentFiles)
            qDebug() << "Would remove" << file;
    }

    if (error)
        error->clear();
    return true;
}

}   

NetAction::Ptr AssetObject::getDownloadAction()
{
    QFileInfo objectFile(getLocalPath());
    if (!fileMatchesAsset(objectFile.filePath(), *this)) {
        auto objectDL = Net::ApiDownload::makeFile(getUrl(), objectFile.filePath());
        if (hash.size()) {
            auto rawHash = QByteArray::fromHex(hash.toLatin1());
            objectDL->addValidator(new Net::ChecksumValidator(QCryptographicHash::Sha1, rawHash));
        }
        objectDL->setProgress(objectDL->getProgress(), size);
        return objectDL;
    }
    return nullptr;
}

QString AssetObject::getLocalPath()
{
    return "assets/objects/" + getRelPath();
}

QUrl AssetObject::getUrl()
{
    return BuildConfig.RESOURCE_BASE + getRelPath();
}

QString AssetObject::getRelPath()
{
    return hash.left(2) + "/" + hash;
}

NetJob::Ptr AssetsIndex::getDownloadJob()
{
    auto job = makeShared<NetJob>(QObject::tr("Assets for %1").arg(id), APPLICATION->network());
    QSet<QString> checkedObjects;
    for (auto& object : objects.values()) {
        const QString localPath = object.getLocalPath();
        if (checkedObjects.contains(localPath))
            continue;
        checkedObjects.insert(localPath);
        auto dl = object.getDownloadAction();
        if (dl) {
            job->addNetAction(dl);
        }
    }
    if (job->size())
        return job;
    return nullptr;
}
