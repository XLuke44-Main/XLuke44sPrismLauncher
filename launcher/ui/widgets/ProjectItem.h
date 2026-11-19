#pragma once

#include <QStyledItemDelegate>

enum UserDataTypes {
    TITLE = 257,         
    DESCRIPTION = 258,   
    SELECTED = 259,      
    INSTALLED = 260      
};

class ProjectItemDelegate final : public QStyledItemDelegate {
    Q_OBJECT

   public:
    ProjectItemDelegate(QWidget* parent);

    void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;
};
