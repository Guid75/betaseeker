//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

#include <QStyle>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QStyle>

#include "linkdelegate.h"

LinkDelegate::LinkDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void LinkDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data(Qt::UserRole + 1).toInt() == 0) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    QString text = opt.text;

    // draw correct background
    opt.text = "";
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QRect rect = opt.rect;
    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        cg = QPalette::Inactive;

    painter->setPen(opt.palette.color(cg, QPalette::Link));
    QFont font = painter->font();
    font.setUnderline(opt.state & QStyle::State_MouseOver);

    text = elidedText(painter->fontMetrics(), rect.width(), option.textElideMode, text);

    painter->setFont(font);
    painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()),
                      opt.displayAlignment, text);
}

bool LinkDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if ((event->type() != QEvent::MouseMove && event->type() != QEvent::MouseButtonRelease) || !index.isValid())
        return false;

    if (index.data(Qt::UserRole + 1).toInt() == 0) {
        if (QApplication::overrideCursor() && QApplication::overrideCursor()->shape() == Qt::PointingHandCursor)
            QApplication::restoreOverrideCursor();

        return false;
    }

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    QRect rect = option.rect;
    QString text = index.data().toString();

    QFontMetrics fontMetrics(option.font);

    QRect r = fontMetrics.boundingRect(rect.left(), rect.top(), rect.width(), rect.height(), 0, text);
    bool intoBounds = mouseEvent->x() >= r.left() && mouseEvent->x() <= r.right() &&
            mouseEvent->y() >= r.top() && mouseEvent->y() <= r.bottom();

    switch (event->type()) {
    case QEvent::MouseMove:
        if (intoBounds) {
                if (!QApplication::overrideCursor() || QApplication::overrideCursor()->shape() != Qt::PointingHandCursor)
                    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
        } else {
            if (QApplication::overrideCursor() && QApplication::overrideCursor()->shape() == Qt::PointingHandCursor)
                QApplication::restoreOverrideCursor();
        }
        return true;
    case QEvent::MouseButtonRelease:
        if (intoBounds) {
            emit linkClicked(index);
        }

        return true;
    default:
        break;
    }

    return false;
}
