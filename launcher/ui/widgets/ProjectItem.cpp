#include "ProjectItem.h"

#include "Common.h"

#include <QIcon>
#include <QPainter>

ProjectItemDelegate::ProjectItemDelegate(QWidget* parent) : QStyledItemDelegate(parent) {}

void ProjectItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    auto rect = opt.rect;

    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(rect, opt.palette.highlight());
        painter->setPen(opt.palette.highlightedText().color());
    } else if (opt.state & QStyle::State_MouseOver) {
        painter->fillRect(rect, opt.palette.window());
    }

    auto icon_width = rect.height(), icon_height = rect.height();
    int icon_x_margin = (rect.height() - icon_width) / 2;
    int icon_y_margin = (rect.height() - icon_height) / 2;

    if (!opt.icon.isNull()) {   
        {
            auto icon_size = opt.decorationSize;
            icon_width = icon_size.width();
            icon_height = icon_size.height();

            icon_y_margin = (rect.height() - icon_height) / 2;
            icon_x_margin = icon_y_margin;   
        }

        int x = rect.x() + icon_x_margin;
        int y = rect.y() + icon_y_margin;

        if (icon_width > 0 && icon_height > 0)
            opt.icon.paint(painter, x, y, icon_width, icon_height);
    }

    auto remaining_width = rect.width() - icon_width - 2 * icon_x_margin;
    rect.setRect(rect.x() + icon_width + 2 * icon_x_margin, rect.y(), remaining_width, rect.height());

    int title_height = 0;

    {   
        auto title = index.data(UserDataTypes::TITLE).toString();

        painter->save();

        auto font = opt.font;
        if (index.data(UserDataTypes::SELECTED).toBool()) {

            font.setBold(true);
            font.setUnderline(true);
        }
        if (index.data(UserDataTypes::INSTALLED).toBool()) {
            auto hRect = opt.rect;
            hRect.setX(hRect.x() + 1);
            hRect.setY(hRect.y() + 1);
            hRect.setHeight(hRect.height() - 2);
            hRect.setWidth(hRect.width() - 2);

            font.setItalic(true);
            font.setOverline(true);
            painter->drawRect(hRect);
        }

        font.setPointSize(font.pointSize() + 2);
        painter->setFont(font);

        title_height = QFontMetrics(font).height();

        painter->drawText(rect.x(), rect.y() + title_height, title);

        painter->restore();
    }

    {   
        auto description = index.data(UserDataTypes::DESCRIPTION).toString().simplified();

        QTextLayout text_layout(description, opt.font);

        qreal height = 0;
        auto cut_text = viewItemTextLayout(text_layout, remaining_width, height);

        description = cut_text.first().second;
        auto num_lines = 1;

        if (cut_text.size() > 1) {

            if (rect.height() - title_height <= 2.5 * opt.fontMetrics.height()) {

                description = opt.fontMetrics.elidedText(description, opt.textElideMode, cut_text.at(0).first);
            } else {
                if (cut_text.size() > 2) {
                    description += opt.fontMetrics.elidedText(cut_text.at(1).second, opt.textElideMode, cut_text.at(1).first);
                } else {
                    description += cut_text.at(1).second;
                }
                num_lines += 1;
            }
        }

        int description_x = rect.x();

        int description_y = rect.y() + title_height + (rect.height() - title_height) / 2;
        if (num_lines == 1)
            description_y -= opt.fontMetrics.height() / 2;
        else
            description_y -= opt.fontMetrics.height();

        painter->drawText(description_x, description_y, remaining_width, cut_text.size() * opt.fontMetrics.height(), Qt::TextWordWrap,
                          description);
    }

    painter->restore();
}
