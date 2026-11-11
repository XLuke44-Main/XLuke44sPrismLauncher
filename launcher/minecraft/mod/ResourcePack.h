#pragma once

#include "Resource.h"

#include <QImage>
#include <QMutex>
#include <QPixmap>
#include <QPixmapCache>

class Version;

class ResourcePack : public Resource {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Resource>;

    ResourcePack(QObject* parent = nullptr) : Resource(parent) {}
    ResourcePack(QFileInfo file_info) : Resource(file_info) {}

    [[nodiscard]] int packFormat() const { return m_pack_format; }

    [[nodiscard]] std::pair<Version, Version> compatibleVersions() const;

    [[nodiscard]] QString description() const { return m_description; }

    [[nodiscard]] QPixmap image(QSize size, Qt::AspectRatioMode mode = Qt::AspectRatioMode::IgnoreAspectRatio) const;

    void setPackFormat(int new_format_id);

    void setDescription(QString new_description);

    void setImage(QImage new_image) const;

    bool valid() const override;

    [[nodiscard]] auto compare(Resource const& other, SortType type) const -> std::pair<int, bool> override;
    [[nodiscard]] bool applyFilter(QRegularExpression filter) const override;

   protected:
    mutable QMutex m_data_lock;

    int m_pack_format = 0;

    QString m_description;

    struct {
        QPixmapCache::Key key;
        bool was_ever_used = false;
    } mutable m_pack_image_cache_key;
};
