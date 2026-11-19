#pragma once

#include <QDateTime>
#include <QFileInfo>
#include <QObject>
#include <QPointer>

#include "QObjectPtr.h"

enum class ResourceType {
    UNKNOWN,      
    ZIPFILE,      
    SINGLEFILE,   
    FOLDER,       
    LITEMOD,      
};

enum class SortType { NAME, DATE, VERSION, ENABLED, PACK_FORMAT, PROVIDER };

enum class EnableAction { ENABLE, DISABLE, TOGGLE };

class Resource : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(Resource)
   public:
    using Ptr = shared_qobject_ptr<Resource>;
    using WeakPtr = QPointer<Resource>;

    Resource(QObject* parent = nullptr);
    Resource(QFileInfo file_info);
    Resource(QString file_path) : Resource(QFileInfo(file_path)) {}

    ~Resource() override = default;

    void setFile(QFileInfo file_info);
    void parseFile();

    [[nodiscard]] auto fileinfo() const -> QFileInfo { return m_file_info; }
    [[nodiscard]] auto dateTimeChanged() const -> QDateTime { return m_changed_date_time; }
    [[nodiscard]] auto internal_id() const -> QString { return m_internal_id; }
    [[nodiscard]] auto type() const -> ResourceType { return m_type; }
    [[nodiscard]] bool enabled() const { return m_enabled; }

    [[nodiscard]] virtual auto name() const -> QString { return m_name; }
    [[nodiscard]] virtual bool valid() const { return m_type != ResourceType::UNKNOWN; }

    [[nodiscard]] virtual auto compare(Resource const& other, SortType type = SortType::NAME) const -> std::pair<int, bool>;

    [[nodiscard]] virtual bool applyFilter(QRegularExpression filter) const;

    bool enable(EnableAction action);

    [[nodiscard]] auto shouldResolve() const -> bool { return !m_is_resolving && !m_is_resolved; }
    [[nodiscard]] auto isResolving() const -> bool { return m_is_resolving; }
    [[nodiscard]] auto isResolved() const -> bool { return m_is_resolved; }
    [[nodiscard]] auto resolutionTicket() const -> int { return m_resolution_ticket; }

    void setResolving(bool resolving, int resolutionTicket)
    {
        m_is_resolving = resolving;
        m_resolution_ticket = resolutionTicket;
    }

    bool destroy(bool attemptTrash = true);

    [[nodiscard]] auto isSymLink() const -> bool { return m_file_info.isSymLink(); }

    [[nodiscard]] bool isSymLinkUnder(const QString& instPath) const;

    [[nodiscard]] bool isMoreThanOneHardLink() const;

   protected:

    QFileInfo m_file_info;

    QDateTime m_changed_date_time;

    QString m_internal_id;

    QString m_name;

    ResourceType m_type = ResourceType::UNKNOWN;

    bool m_enabled = true;

    bool m_is_resolving = false;
    bool m_is_resolved = false;
    int m_resolution_ticket = 0;
};
