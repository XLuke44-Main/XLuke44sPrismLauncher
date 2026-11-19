#ifndef QUAZIP_QT_COMPAT_H
#define QUAZIP_QT_COMPAT_H

#include <QtCore/Qt>
#include <QtCore/QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QtCore5Compat/QTextCodec>
#else
#  include <QtCore/QTextCodec>
#endif

#if (QT_VERSION >= 0x050100)
#include <QtCore/QSaveFile>
inline bool quazip_close(QIODevice *device) {
    QSaveFile *file = qobject_cast<QSaveFile*>(device);
    if (file != nullptr) {

        return file->commit();
    }
    device->close();
    return true;
}
#else
inline bool quazip_close(QIODevice *device) {
    device->close();
    return true;
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
using Qt::SkipEmptyParts;
#else
#include <QtCore/QString>
const auto SkipEmptyParts = QString::SplitBehavior::SkipEmptyParts;
#endif

#include <QtCore/QList>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
#include <algorithm>
template<typename T, typename C>
inline void quazip_sort(T begin, T end, C comparator) {
    std::sort(begin, end, comparator);
}
#else
#include <QtCore/QtAlgorithms>
template<typename T, typename C>
inline void quazip_sort(T begin, T end, C comparator) {
    qSort(begin, end, comparator);
}
#endif

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
inline QDateTime quazip_ctime(const QFileInfo &fi) {
    return fi.birthTime();
}
#else
inline QDateTime quazip_ctime(const QFileInfo &fi) {
    return fi.created();
}
#endif

#include <QtCore/QFileInfo>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
inline bool quazip_is_symlink(const QFileInfo &fi) {
    return fi.isSymbolicLink();
}
#else
inline bool quazip_is_symlink(const QFileInfo &fi) {

    return fi.isSymLink();
}
#endif

#include <QtCore/QFileInfo>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
inline QString quazip_symlink_target(const QFileInfo &fi) {
    return fi.symLinkTarget();
}
#else
inline QString quazip_symlink_target(const QFileInfo &fi) {
    return fi.readLink();  
}
#endif

#include <QtCore/QDateTime>
#if (QT_VERSION >= 0x040700)
inline quint64 quazip_ntfs_ticks(const QDateTime &time, int fineTicks) {
    QDateTime base(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    return base.msecsTo(time) * 10000 + fineTicks;
}
#else
inline quint64 quazip_ntfs_ticks(const QDateTime &time, int fineTicks) {
    QDateTime base(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    QDateTime utc = time.toUTC();
    return (static_cast<qint64>(base.date().daysTo(utc.date()))
            * Q_INT64_C(86400000)
            + static_cast<qint64>(base.time().msecsTo(utc.time())))
        * Q_INT64_C(10000) + fineTicks;
}
#endif

#include <QtCore/QDateTime>
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)  
inline qint64 quazip_to_time64_t(const QDateTime &time) {
    return time.toSecsSinceEpoch();
}
#else
inline qint64 quazip_to_time64_t(const QDateTime &time) {
    return static_cast<qint64>(time.toTime_t());  
}
#endif

#include <QtCore/QTextStream>

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
const auto quazip_endl = Qt::endl;
#else
const auto quazip_endl = endl;
#endif

#endif  
