
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "BaseInstance.h"

#include "modplatform/modrinth/ModrinthAPI.h"
#include "modplatform/modrinth/ModrinthPackManifest.h"

#include "modplatform/flame/FlameAPI.h"
#include "modplatform/flame/FlamePackIndex.h"

#include "net/NetJob.h"

#include "ui/pages/BasePage.h"

#include <QWidget>

namespace Ui {
class ManagedPackPage;
}

class InstanceTask;
class InstanceWindow;

class ManagedPackPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    inline static ManagedPackPage* createPage(BaseInstance* inst, QWidget* parent = nullptr)
    {
        return ManagedPackPage::createPage(inst, inst->getManagedPackType(), parent);
    }

    static ManagedPackPage* createPage(BaseInstance* inst, QString type, QWidget* parent = nullptr);
    ~ManagedPackPage() override;

    [[nodiscard]] QString displayName() const override;
    [[nodiscard]] QIcon icon() const override;
    [[nodiscard]] QString helpPage() const override;
    [[nodiscard]] QString id() const override { return "managed_pack"; }
    [[nodiscard]] bool shouldDisplay() const override;

    void openedImpl() override;

    bool apply() override { return true; }
    void retranslate() override;

    virtual void parseManagedPack(){};

    [[nodiscard]] virtual QString url() const { return {}; };

    void setInstanceWindow(InstanceWindow* window) { m_instance_window = window; }

   public slots:

    virtual void suggestVersion();

    virtual void update(){};
    virtual void updateFromFile(){};

   protected slots:

    void setFailState();

   protected:
    ManagedPackPage(BaseInstance* inst, InstanceWindow* instance_window, QWidget* parent = nullptr);

    bool runUpdateTask(InstanceTask*);

   protected:
    InstanceWindow* m_instance_window = nullptr;

    Ui::ManagedPackPage* ui;
    BaseInstance* m_inst;

    bool m_loaded = false;
};

class GenericManagedPackPage final : public ManagedPackPage {
    Q_OBJECT

   public:
    GenericManagedPackPage(BaseInstance* inst, InstanceWindow* instance_window, QWidget* parent = nullptr)
        : ManagedPackPage(inst, instance_window, parent)
    {}
    ~GenericManagedPackPage() override = default;

    [[nodiscard]] bool shouldDisplay() const override { return false; };
};

class ModrinthManagedPackPage final : public ManagedPackPage {
    Q_OBJECT

   public:
    ModrinthManagedPackPage(BaseInstance* inst, InstanceWindow* instance_window, QWidget* parent = nullptr);
    ~ModrinthManagedPackPage() override = default;

    void parseManagedPack() override;
    [[nodiscard]] QString url() const override;

   public slots:
    void suggestVersion() override;

    void update() override;
    void updateFromFile() override;

   private:
    NetJob::Ptr m_fetch_job = nullptr;

    Modrinth::Modpack m_pack;
    ModrinthAPI m_api;
};

class FlameManagedPackPage final : public ManagedPackPage {
    Q_OBJECT

   public:
    FlameManagedPackPage(BaseInstance* inst, InstanceWindow* instance_window, QWidget* parent = nullptr);
    ~FlameManagedPackPage() override = default;

    void parseManagedPack() override;
    [[nodiscard]] QString url() const override;

   public slots:
    void suggestVersion() override;

    void update() override;
    void updateFromFile() override;

   private:
    NetJob::Ptr m_fetch_job = nullptr;

    Flame::IndexedPack m_pack;
    FlameAPI m_api;
};
