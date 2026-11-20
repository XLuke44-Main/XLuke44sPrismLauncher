#pragma once

#include "BaseVersion.h"
#include "InstanceTask.h"

class InstanceCreationTask : public InstanceTask {
    Q_OBJECT
   public:
    InstanceCreationTask();
    virtual ~InstanceCreationTask() = default;

   protected:
    void executeTask() final override;

    virtual bool updateInstance() { return false; };

    virtual bool createInstance() { return false; };

    QString getError() const { return m_error_message; }

   protected:
    void setError(const QString& message) { m_error_message = message; };

   protected:
    bool m_abort = false;

    QStringList m_files_to_remove;

   private:
    QString m_error_message;
};
