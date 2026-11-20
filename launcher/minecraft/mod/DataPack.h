
// SPDX-License-Identifier: GPL-3.0-only

/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 */

#pragma once

#include "Resource.h"

#include <QMutex>

class Version;

class DataPack : public Resource {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Resource>;

    DataPack(QObject* parent = nullptr) : Resource(parent) {}
    DataPack(QFileInfo file_info) : Resource(file_info) {}

    [[nodiscard]] int packFormat() const { return m_pack_format; }

    [[nodiscard]] std::pair<Version, Version> compatibleVersions() const;

    [[nodiscard]] QString description() const { return m_description; }

    void setPackFormat(int new_format_id);

    void setDescription(QString new_description);

    bool valid() const override;

    [[nodiscard]] auto compare(Resource const& other, SortType type) const -> std::pair<int, bool> override;
    [[nodiscard]] bool applyFilter(QRegularExpression filter) const override;

   protected:
    mutable QMutex m_data_lock;

    int m_pack_format = 0;

    QString m_description;
};
