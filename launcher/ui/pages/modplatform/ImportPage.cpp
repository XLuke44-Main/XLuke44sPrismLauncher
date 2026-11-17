// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
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

#include "ImportPage.h"

#include "ui/dialogs/ProgressDialog.h"
#include "ui_ImportPage.h"

#include <QFileDialog>
#include <QValidator>
#include <utility>

#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/NewInstanceDialog.h"

#include "modplatform/flame/FlameAPI.h"

#include "Json.h"

#include "InstanceImportTask.h"

class UrlValidator : public QValidator {
   public:
    using QValidator::QValidator;

    State validate(QString& in, [[maybe_unused]] int& pos) const
    {
        const QUrl url(in);
        if (url.isValid() && !url.isRelative() && !url.isEmpty()) {
            return Acceptable;
        } else if (QFile::exists(in)) {
            return Acceptable;
        } else {
            return Intermediate;
        }
    }
};

ImportPage::ImportPage(NewInstanceDialog* dialog, QWidget* parent) : QWidget(parent), ui(new Ui::ImportPage), dialog(dialog)
{
    ui->setupUi(this);
    ui->modpackEdit->setValidator(new UrlValidator(ui->modpackEdit));
    connect(ui->modpackEdit, &QLineEdit::textChanged, this, &ImportPage::updateState);
}

ImportPage::~ImportPage()
{
    delete ui;
}

bool ImportPage::shouldDisplay() const
{
    return true;
}

void ImportPage::retranslate()
{
    ui->retranslateUi(this);
}

void ImportPage::openedImpl()
{
    updateState();
}

void ImportPage::updateState()
{
    if (!isOpened) {
        return;
    }
    if (ui->modpackEdit->hasAcceptableInput()) {
        QString input = ui->modpackEdit->text();
        auto url = QUrl::fromUserInput(input);
        if (url.isLocalFile()) {

            QFileInfo fi(input);

            bool isZip = QMimeDatabase().mimeTypeForUrl(url).suffixes().contains("zip");

            bool isMRPack = fi.suffix() == "mrpack";

            if (fi.exists() && (isZip || isMRPack)) {
                auto extra_info = QMap(m_extra_info);
                qDebug() << "Pack Extra Info" << extra_info << m_extra_info;
                dialog->setSuggestedPack(fi.completeBaseName(), new InstanceImportTask(url, this, std::move(extra_info)));
                dialog->setSuggestedIcon("default");
            }
        } else if (url.scheme() == "curseforge") {

            QUrlQuery query(url);
            const auto addonIds = query.allQueryItemValues("addonId");
            const auto fileIds = query.allQueryItemValues("fileId");
            bool addonIdOk = false;
            bool fileIdOk = false;
            const auto addonId = addonIds.isEmpty() ? QString() : addonIds.front();
            const auto fileId = fileIds.isEmpty() ? QString() : fileIds.front();
            addonId.toInt(&addonIdOk);
            fileId.toInt(&fileIdOk);
            if (addonIds.size() != 1 || fileIds.size() != 1 || !addonIdOk || !fileIdOk || addonId.toInt() <= 0 || fileId.toInt() <= 0) {
                CustomMessageBox::selectable(
                    this, tr("Invalid CurseForge link"),
                    tr("A CurseForge install link must include positive numeric addonId and fileId values."),
                    QMessageBox::Critical)
                    ->show();
                return;
            }
            auto array = std::make_shared<QByteArray>();

            auto api = FlameAPI();
            auto job = api.getFile(addonId, fileId, array);

            connect(job.get(), &NetJob::failed, this,
                    [this](QString reason) { CustomMessageBox::selectable(this, tr("Error"), reason, QMessageBox::Critical)->show(); });
            connect(job.get(), &NetJob::succeeded, this, [this, array, addonId, fileId] {
                qDebug() << "Returned CFURL Json:\n" << array->toStdString().c_str();
                try {
                    auto doc = Json::requireDocument(*array);
                    auto data = Json::ensureObject(doc.object(), "data");

                    auto fileName = Json::requireString(data, "fileName");
                    if (!fileName.endsWith(".zip", Qt::CaseInsensitive)) {
                        CustomMessageBox::selectable(this, tr("Error"), tr("This CurseForge file is not a modpack ZIP."),
                                                     QMessageBox::Critical)
                            ->show();
                        return;
                    }

                    auto dl_url = QUrl(Json::ensureString(data, "downloadUrl"));
                    if (!dl_url.isValid() || dl_url.isEmpty() || dl_url.scheme() != "https" || dl_url.host().isEmpty()) {
                        CustomMessageBox::selectable(
                            this, tr("Error"),
                            tr("The modpack %1 is blocked for third-party downloads. Download it manually from CurseForge.")
                                .arg(fileName),
                            QMessageBox::Critical)
                            ->show();
                        return;
                    }

                    QFileInfo dl_file(dl_url.fileName());
                    QString pack_name = Json::ensureString(data, "displayName", dl_file.completeBaseName(), "displayName");

                    QMap<QString, QString> extra_info;
                    extra_info.insert("pack_id", addonId);
                    extra_info.insert("pack_version_id", fileId);

                    dialog->setSuggestedPack(pack_name, new InstanceImportTask(dl_url, this, std::move(extra_info)));
                    dialog->setSuggestedIcon("default");
                } catch (const JSONValidationError& e) {
                    CustomMessageBox::selectable(this, tr("Error"),
                                                 tr("CurseForge returned an invalid file response:\n%1").arg(e.cause()),
                                                 QMessageBox::Critical)
                        ->show();
                }
            });
            ProgressDialog dlUrlDialod(this);
            dlUrlDialod.setSkipButton(true, tr("Abort"));
            dlUrlDialod.execWithTask(job.get());
            return;
        } else {
            if (input.endsWith("?client=y")) {
                input.chop(9);
                input.append("/file");
                url = QUrl::fromUserInput(input);
            }

            QFileInfo fi(url.fileName());
            auto extra_info = QMap(m_extra_info);
            dialog->setSuggestedPack(fi.completeBaseName(), new InstanceImportTask(url, this, std::move(extra_info)));
            dialog->setSuggestedIcon("default");
        }
    } else {
        dialog->setSuggestedPack();
    }
}

void ImportPage::setUrl(const QString& url)
{
    ui->modpackEdit->setText(url);
    updateState();
}

void ImportPage::setExtraInfo(const QMap<QString, QString>& extra_info)
{
    m_extra_info = extra_info;
    updateState();
}

void ImportPage::on_modpackBtn_clicked()
{
    const QMimeType zip = QMimeDatabase().mimeTypeForName("application/zip");
    auto filter = tr("Supported files") + QString(" (%1 *.mrpack)").arg(zip.globPatterns().join(" "));
    filter += ";;" + zip.filterString();

    filter += ";;" + tr("Modrinth pack") + " (*.mrpack)";
    const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Choose modpack"), modpackUrl(), filter);
    if (url.isValid()) {
        if (url.isLocalFile()) {
            ui->modpackEdit->setText(url.toLocalFile());
        } else {
            ui->modpackEdit->setText(url.toString());
        }
    }
}

QUrl ImportPage::modpackUrl() const
{
    const QUrl url(ui->modpackEdit->text());
    if (url.isValid() && !url.isRelative() && !url.host().isEmpty()) {
        return url;
    } else {
        return QUrl::fromLocalFile(ui->modpackEdit->text());
    }
}
