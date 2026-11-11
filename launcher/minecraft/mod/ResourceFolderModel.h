#pragma once

#include <QAbstractListModel>
#include <QAction>
#include <QDir>
#include <QFileSystemWatcher>
#include <QHeaderView>
#include <QMutex>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QTreeView>

#include "Resource.h"

#include "BaseInstance.h"

#include "tasks/ConcurrentTask.h"
#include "tasks/Task.h"

class QSortFilterProxyModel;

class ResourceFolderModel : public QAbstractListModel {
    Q_OBJECT
   public:
    ResourceFolderModel(QDir, BaseInstance* instance, QObject* parent = nullptr, bool create_dir = true);
    ~ResourceFolderModel() override;

    virtual QString id() const { return "resource"; }

    bool startWatching(const QStringList& paths);

    bool stopWatching(const QStringList& paths);

    virtual bool startWatching() { return startWatching({ m_dir.absolutePath() }); }
    virtual bool stopWatching() { return stopWatching({ m_dir.absolutePath() }); }

    virtual bool installResource(QString path);

    virtual bool uninstallResource(QString file_name);
    virtual bool deleteResources(const QModelIndexList&);

    virtual bool setResourceEnabled(const QModelIndexList& indexes, EnableAction action);

    virtual bool update();

    virtual void resolveResource(Resource* res);

    [[nodiscard]] qsizetype size() const { return m_resources.size(); }
    [[nodiscard]] bool empty() const { return size() == 0; }
    [[nodiscard]] Resource& at(int index) { return *m_resources.at(index); }
    [[nodiscard]] Resource const& at(int index) const { return *m_resources.at(index); }
    [[nodiscard]] QList<Resource::Ptr> const& all() const { return m_resources; }

    [[nodiscard]] QDir const& dir() const { return m_dir; }

    [[nodiscard]] bool hasPendingParseTasks() const;

    enum Columns { ACTIVE_COLUMN = 0, NAME_COLUMN, DATE_COLUMN, NUM_COLUMNS };
    QStringList columnNames(bool translated = true) const { return translated ? m_column_names_translated : m_column_names; }

    [[nodiscard]] int rowCount(const QModelIndex& parent = {}) const override { return parent.isValid() ? 0 : static_cast<int>(size()); }
    [[nodiscard]] int columnCount(const QModelIndex& parent = {}) const override { return parent.isValid() ? 0 : NUM_COLUMNS; }

    [[nodiscard]] Qt::DropActions supportedDropActions() const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QStringList mimeTypes() const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    [[nodiscard]] bool validateIndex(const QModelIndex& index) const;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setupHeaderAction(QAction* act, int column);
    void saveColumns(QTreeView* tree);
    void loadColumns(QTreeView* tree);
    QMenu* createHeaderContextMenu(QTreeView* tree);

    QSortFilterProxyModel* createFilterProxyModel(QObject* parent = nullptr);

    [[nodiscard]] SortType columnToSortKey(size_t column) const;
    [[nodiscard]] QList<QHeaderView::ResizeMode> columnResizeModes() const { return m_column_resize_modes; }

    class ProxyModel : public QSortFilterProxyModel {
       public:
        explicit ProxyModel(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}

       protected:
        [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
        [[nodiscard]] bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
    };

    QString instDirPath() const;

   signals:
    void updateFinished();

   protected:

    [[nodiscard]] virtual Task* createUpdateTask();

    [[nodiscard]] virtual Task* createParseTask(Resource&) { return nullptr; }

    template <typename T>
    void applyUpdates(QSet<QString>& current_set, QSet<QString>& new_set, QMap<QString, T>& new_resources);

   protected slots:
    void directoryChanged(QString);

    virtual void onUpdateSucceeded();
    virtual void onUpdateFailed() {}

    virtual void onParseSucceeded(int ticket, QString resource_id);
    virtual void onParseFailed(int ticket, QString resource_id)
    {
        Q_UNUSED(ticket);
        Q_UNUSED(resource_id);
    }

   protected:

    QList<SortType> m_column_sort_keys = { SortType::ENABLED, SortType::NAME, SortType::DATE };
    QStringList m_column_names = { "Enable", "Name", "Last Modified" };
    QStringList m_column_names_translated = { tr("Enable"), tr("Name"), tr("Last Modified") };
    QList<QHeaderView::ResizeMode> m_column_resize_modes = { QHeaderView::Interactive, QHeaderView::Stretch, QHeaderView::Interactive };
    QList<bool> m_columnsHideable = { false, false, true };

    QDir m_dir;
    BaseInstance* m_instance;
    QFileSystemWatcher m_watcher;
    bool m_is_watching = false;

    Task::Ptr m_current_update_task = nullptr;
    bool m_scheduled_update = false;

    QList<Resource::Ptr> m_resources;

    QMap<QString, int> m_resources_index;

    ConcurrentTask m_helper_thread_task;
    QMap<int, Task::Ptr> m_active_parse_tasks;
    std::atomic<int> m_next_resolution_ticket = 0;
};

#define RESOURCE_HELPERS(T)                                                                       \
    [[nodiscard]] T* operator[](int index)                                                        \
    {                                                                                             \
        return static_cast<T*>(m_resources[index].get());                                         \
    }                                                                                             \
    [[nodiscard]] T* at(int index)                                                                \
    {                                                                                             \
        return static_cast<T*>(m_resources[index].get());                                         \
    }                                                                                             \
    [[nodiscard]] const T* at(int index) const                                                    \
    {                                                                                             \
        return static_cast<const T*>(m_resources.at(index).get());                                \
    }                                                                                             \
    [[nodiscard]] T* first()                                                                      \
    {                                                                                             \
        return static_cast<T*>(m_resources.first().get());                                        \
    }                                                                                             \
    [[nodiscard]] T* last()                                                                       \
    {                                                                                             \
        return static_cast<T*>(m_resources.last().get());                                         \
    }                                                                                             \
    [[nodiscard]] T* find(QString id)                                                             \
    {                                                                                             \
        auto iter = std::find_if(m_resources.constBegin(), m_resources.constEnd(),                \
                                 [&](Resource::Ptr const& r) { return r->internal_id() == id; }); \
        if (iter == m_resources.constEnd())                                                       \
            return nullptr;                                                                       \
        return static_cast<T*>((*iter).get());                                                    \
    }

template <typename T>
void ResourceFolderModel::applyUpdates(QSet<QString>& current_set, QSet<QString>& new_set, QMap<QString, T>& new_resources)
{

    {
        QSet<QString> kept_set = current_set;
        kept_set.intersect(new_set);

        for (auto const& kept : kept_set) {
            auto row_it = m_resources_index.constFind(kept);
            Q_ASSERT(row_it != m_resources_index.constEnd());
            auto row = row_it.value();

            auto& new_resource = new_resources[kept];
            auto const& current_resource = m_resources.at(row);

            if (new_resource->dateTimeChanged() == current_resource->dateTimeChanged()) {

                continue;
            }

            if (current_resource->isResolving()) {
                auto ticket = current_resource->resolutionTicket();
                if (m_active_parse_tasks.contains(ticket)) {
                    auto task = (*m_active_parse_tasks.find(ticket)).get();
                    task->abort();
                }
            }

            m_resources[row].reset(new_resource);
            resolveResource(m_resources.at(row).get());
            emit dataChanged(index(row, 0), index(row, columnCount(QModelIndex()) - 1));
        }
    }

    {
        QSet<QString> removed_set = current_set;
        removed_set.subtract(new_set);

        QList<int> removed_rows;
        for (auto& removed : removed_set)
            removed_rows.append(m_resources_index[removed]);

        std::sort(removed_rows.begin(), removed_rows.end(), std::greater<int>());

        for (auto& removed_index : removed_rows) {
            auto removed_it = m_resources.begin() + removed_index;

            Q_ASSERT(removed_it != m_resources.end());

            if ((*removed_it)->isResolving()) {
                auto ticket = (*removed_it)->resolutionTicket();
                if (m_active_parse_tasks.contains(ticket)) {
                    auto task = (*m_active_parse_tasks.find(ticket)).get();
                    task->abort();
                }
            }

            beginRemoveRows(QModelIndex(), removed_index, removed_index);
            m_resources.erase(removed_it);
            endRemoveRows();
        }
    }

    {
        QSet<QString> added_set = new_set;
        added_set.subtract(current_set);

        if (added_set.size() > 0) {
            beginInsertRows(QModelIndex(), static_cast<int>(m_resources.size()),
                            static_cast<int>(m_resources.size() + added_set.size() - 1));

            for (auto& added : added_set) {
                auto res = new_resources[added];
                m_resources.append(res);
                resolveResource(m_resources.last().get());
            }

            endInsertRows();
        }
    }

    {
        m_resources_index.clear();
        int idx = 0;
        for (auto const& mod : qAsConst(m_resources)) {
            m_resources_index[mod->internal_id()] = idx;
            idx++;
        }
    }
}
