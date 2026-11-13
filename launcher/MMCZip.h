// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *  Copyright (c) 2023 Trial97 <alexandru.tripon97@gmail.com>
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

#include <quazip.h>
#include <quazip/JlCompress.h>
#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QSet>
#include <QString>
#include <functional>
#include <memory>
#include <optional>

#if defined(LAUNCHER_APPLICATION)
#include "minecraft/mod/Mod.h"
#endif
#include "tasks/Task.h"

namespace MMCZip {
using FilterFunction = std::function<bool(const QString&)>;

bool mergeZipFiles(QuaZip* into, QFileInfo from, QSet<QString>& contained, const FilterFunction& filter = nullptr);

bool compressDirFiles(QuaZip* zip, QString dir, QFileInfoList files, bool followSymlinks = false);

bool compressDirFiles(QString fileCompressed, QString dir, QFileInfoList files, bool followSymlinks = false);

#if defined(LAUNCHER_APPLICATION)

bool createModdedJar(QString sourceJarPath, QString targetJarPath, const QList<Mod*>& mods);
#endif

QString findFolderOfFileInZip(QuaZip* zip, const QString& what, const QStringList& ignore_paths = {}, const QString& root = QString(""));

bool findFilesInZip(QuaZip* zip, const QString& what, QStringList& result, const QString& root = QString());

std::optional<QStringList> extractSubDir(QuaZip* zip, const QString& subdir, const QString& target);

bool extractRelFile(QuaZip* zip, const QString& file, const QString& target);

std::optional<QStringList> extractDir(QString fileCompressed, QString dir);

std::optional<QStringList> extractDir(QString fileCompressed, QString subdir, QString dir);

bool extractFile(QString fileCompressed, QString file, QString dir);

bool collectFileListRecursively(const QString& rootDir, const QString& subDir, QFileInfoList* files, FilterFunction excludeFilter);

#if defined(LAUNCHER_APPLICATION)
class ExportToZipTask : public Task {
   public:
    ExportToZipTask(QString outputPath,
                    QDir dir,
                    QFileInfoList files,
                    QString destinationPrefix = "",
                    bool followSymlinks = false,
                    bool utf8Enabled = false)
        : m_output_path(outputPath)
        , m_output(outputPath)
        , m_dir(dir)
        , m_files(files)
        , m_destination_prefix(destinationPrefix)
        , m_follow_symlinks(followSymlinks)
    {
        setAbortable(true);
        m_output.setUtf8Enabled(utf8Enabled);
    };
    ExportToZipTask(QString outputPath,
                    QString dir,
                    QFileInfoList files,
                    QString destinationPrefix = "",
                    bool followSymlinks = false,
                    bool utf8Enabled = false)
        : ExportToZipTask(outputPath, QDir(dir), files, destinationPrefix, followSymlinks, utf8Enabled){};

    virtual ~ExportToZipTask() = default;

    void setExcludeFiles(QStringList excludeFiles) { m_exclude_files = excludeFiles; }
    void addExtraFile(QString fileName, QByteArray data) { m_extra_files.insert(fileName, data); }

    using ZipResult = std::optional<QString>;

   protected:
    virtual void executeTask() override;
    bool abort() override;

    ZipResult exportZip();
    void finish();

   private:
    QString m_output_path;
    QuaZip m_output;
    QDir m_dir;
    QFileInfoList m_files;
    QString m_destination_prefix;
    bool m_follow_symlinks;
    QStringList m_exclude_files;
    QHash<QString, QByteArray> m_extra_files;

    QFuture<ZipResult> m_build_zip_future;
    QFutureWatcher<ZipResult> m_build_zip_watcher;
};
#endif
}   
