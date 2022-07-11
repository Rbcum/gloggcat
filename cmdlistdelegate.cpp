
#include "cmdlistdelegate.h"
#include "cmdlistmodel.h"
#include "qapplication.h"

#include <QMouseEvent>
#include <QPainter>

#define BTN_WIDTH 60
#define BTN_HEIGHT 30
#define MARGIN 5

CmdListDelegate::CmdListDelegate(QObject *parent)
    : QStyledItemDelegate { parent }
{
    containerView = qobject_cast<QListView *>(parent);
    containerView->setMouseTracking(true);
    containerView->viewport()->installEventFilter(this);
    btnState = QStyle::State_None;
}

void CmdListDelegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = "";
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    // Text
    QString text = index.data().toString();
    QRect textRect = rect.adjusted(MARGIN, MARGIN, -BTN_WIDTH - 3 * MARGIN, -MARGIN);
    QPalette::ColorRole textRole
        = opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text;
    style->drawItemText(painter, textRect, Qt::AlignVCenter, opt.palette, true, text, textRole);

    // Button
    QRect buttonRect(rect.right() - BTN_WIDTH - MARGIN,
        rect.top() + (rect.height() - BTN_HEIGHT) / 2, BTN_WIDTH, BTN_HEIGHT);
    QStyleOptionButton button;
    button.rect = buttonRect;
    if (index.data(StateRole).toBool()) {
        button.text = "Stop";
        button.palette.setColor(QPalette::ButtonText, QColorConstants::Svg::red);
    } else {
        button.text = "Start";
        button.palette.setColor(QPalette::ButtonText, QColorConstants::Svg::green);
    }
    button.state = btnState | QStyle::State_Enabled;
    style->drawControl(QStyle::CE_PushButton, &button, painter);
}

QSize CmdListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(0, BTN_HEIGHT + 2 * MARGIN);
    //    return QStyledItemDelegate::sizeHint(option, index);
}

bool CmdListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
    const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect rect = option.rect;
    QRect buttonRect(rect.right() - BTN_WIDTH - MARGIN,
        rect.top() + (rect.height() - BTN_HEIGHT) / 2, BTN_WIDTH, BTN_HEIGHT);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (!buttonRect.contains(mouseEvent->pos())) {
        btnState = QStyle::State_None;
        return false;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        btnState = QStyle::State_Sunken;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        btnState = QStyle::State_Raised;
        emit buttonClicked(index);
    } else {
        btnState = QStyle::State_Raised;
    }
    return true;
}

bool CmdListDelegate::eventFilter(QObject *watched, QEvent *event)
{
    // Disable button double click
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->pos().x() > containerView->width() - BTN_WIDTH - 2 * MARGIN) {
            return true;
        }
    }
    return QStyledItemDelegate::eventFilter(watched, event);
}
