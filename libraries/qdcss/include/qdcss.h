
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef QDCSS_H
#define QDCSS_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <optional>

class QDCSS {

   public:
    QDCSS(QString);
    std::optional<QString>* get(QString);

   private:
    QMap<QString, QStringList> m_data;
};

#endif   
