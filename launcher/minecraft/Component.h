#pragma once

#include <QDateTime>
#include <QJsonDocument>
#include <QList>
#include <memory>
#include "ProblemProvider.h"
#include "QObjectPtr.h"
#include "meta/JsonFormat.h"

class PackProfile;
class LaunchProfile;
namespace Meta {
class Version;
class VersionList;
}   
class VersionFile;

class Component : public QObject, public ProblemProvider {
    Q_OBJECT
   public:
    Component(PackProfile* parent, const QString& uid);

    Component(PackProfile* parent, std::shared_ptr<Meta::Version> version);
    Component(PackProfile* parent, const QString& uid, std::shared_ptr<VersionFile> file);

    virtual ~Component() {}

    void applyTo(LaunchProfile* profile);

    bool isEnabled();
    bool setEnabled(bool state);
    bool canBeDisabled();

    bool isMoveable();
    bool isCustomizable();
    bool isRevertible();
    bool isRemovable();
    bool isCustom();
    bool isVersionChangeable();

    void setOrder(int order);
    int getOrder();

    QString getID();
    QString getName();
    QString getVersion();
    std::shared_ptr<Meta::Version> getMeta();
    QDateTime getReleaseDateTime();

    QString getFilename();

    std::shared_ptr<class VersionFile> getVersionFile() const;
    std::shared_ptr<class Meta::VersionList> getVersionList() const;

    void setImportant(bool state);

    const QList<PatchProblem> getProblems() const override;
    ProblemSeverity getProblemSeverity() const override;

    void setVersion(const QString& version);
    bool customize();
    bool revert();

    void updateCachedData();

   signals:
    void dataChanged();

   public:  
    PackProfile* m_parent;

    QString m_uid;

    QString m_version;

    bool m_dependencyOnly = false;

    bool m_important = false;

    bool m_disabled = false;

    QString m_cachedName;

    QString m_cachedVersion;

    Meta::RequireSet m_cachedRequires;
    Meta::RequireSet m_cachedConflicts;

    bool m_cachedVolatile = false;

    bool m_orderOverride = false;
    int m_order = 0;

    std::shared_ptr<Meta::Version> m_metaVersion;
    std::shared_ptr<VersionFile> m_file;
    bool m_loaded = false;
};

using ComponentPtr = shared_qobject_ptr<Component>;
