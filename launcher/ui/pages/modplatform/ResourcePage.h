
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QTimer>
#include <QWidget>

#include "ResourceDownloadTask.h"
#include "modplatform/ModIndex.h"
#include "modplatform/ResourceAPI.h"

#include "ui/pages/BasePage.h"
#include "ui/pages/modplatform/ResourceModel.h"
#include "ui/widgets/ProgressWidget.h"

namespace Ui {
class ResourcePage;
}

class BaseInstance;

namespace ResourceDownload {

class ResourceDownloadDialog;
class ResourceModel;

class ResourcePage : public QWidget, public BasePage {
    Q_OBJECT
   public:
    using DownloadTaskPtr = shared_qobject_ptr<ResourceDownloadTask>;
    ~ResourcePage() override;

    [[nodiscard]] auto displayName() const -> QString override = 0;
    [[nodiscard]] auto icon() const -> QIcon override = 0;
    [[nodiscard]] auto id() const -> QString override = 0;
    [[nodiscard]] auto helpPage() const -> QString override = 0;
    [[nodiscard]] bool shouldDisplay() const override = 0;

    [[nodiscard]] virtual auto metaEntryBase() const -> QString = 0;
    [[nodiscard]] virtual auto debugName() const -> QString = 0;

    [[nodiscard]] virtual inline QString resourcesString() const { return tr("resources"); }

    [[nodiscard]] virtual inline QString resourceString() const { return tr("resource"); }

    [[nodiscard]] virtual bool supportsFiltering() const = 0;

    void retranslate() override;
    void openedImpl() override;
    auto eventFilter(QObject* watched, QEvent* event) -> bool override;

    [[nodiscard]] auto getSearchTerm() const -> QString;

    void setSearchTerm(QString);

    [[nodiscard]] bool setCurrentPack(ModPlatform::IndexedPack::Ptr);
    [[nodiscard]] auto getCurrentPack() const -> ModPlatform::IndexedPack::Ptr;
    [[nodiscard]] auto getDialog() const -> const ResourceDownloadDialog* { return m_parent_dialog; }
    [[nodiscard]] auto getModel() const -> ResourceModel* { return m_model; }

   protected:
    ResourcePage(ResourceDownloadDialog* parent, BaseInstance&);

    void addSortings();

   public slots:
    virtual void updateUi();
    virtual void updateSelectionButton();
    virtual void updateVersionList();

    void addResourceToDialog(ModPlatform::IndexedPack::Ptr, ModPlatform::IndexedVersion&);
    void removeResourceFromDialog(const QString& pack_name);
    virtual void removeResourceFromPage(const QString& name);
    virtual void addResourceToPage(ModPlatform::IndexedPack::Ptr, ModPlatform::IndexedVersion&, std::shared_ptr<ResourceFolderModel>);

    QList<DownloadTaskPtr> selectedPacks() { return m_model->selectedPacks(); }
    bool hasSelectedPacks() { return !(m_model->selectedPacks().isEmpty()); }

   protected slots:
    virtual void triggerSearch() {}

    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onVersionSelectionChanged(QString data);
    void onResourceSelected();

    virtual QMap<QString, QString> urlHandlers() const = 0;
    virtual void openUrl(const QUrl&);

    virtual bool optedOut(ModPlatform::IndexedVersion& ver) const
    {
        Q_UNUSED(ver);
        return false;
    };

   public:
    BaseInstance& m_base_instance;

   protected:
    Ui::ResourcePage* m_ui;

    ResourceDownloadDialog* m_parent_dialog = nullptr;
    ResourceModel* m_model = nullptr;

    int m_selected_version_index = -1;

    ProgressWidget m_fetch_progress;

    QTimer m_search_timer;
};

}   
