#ifndef CMDLISTDELEGATE_H
#define CMDLISTDELEGATE_H

#include <QListView>
#include <QStyledItemDelegate>

class CmdListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CmdListDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
        const QModelIndex &index) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QListView *containerView;
    QStyle::State btnState;

signals:
    void buttonClicked(const QModelIndex &index);
};

#endif // CMDLISTDELEGATE_H
