#ifndef CMDLISTMODEL_H
#define CMDLISTMODEL_H

#include <QAbstractTableModel>
#include <QProcess>

struct Cmd
{
    int id;
    QString name;
    QString exec;
    int pos;
    ~Cmd();
};
QDebug operator<<(QDebug dbg, const Cmd &c);

enum CustomRoles
{
    DataRole = Qt::UserRole,
    FileRole,
    StateRole,
};

class CmdListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CmdListModel(QObject *parent = nullptr);

private slots:
    void procStateChanged(QProcess::ProcessState state);

private:
    QList<Cmd> cmdList;
    QMap<int, QProcess *> procMap;

    void assignPositions();
    void toggleProc(const QModelIndex &index);
    void updateDB(const Cmd &cmd);
    void insertDB(Cmd &cmd);
    void deleteDB(const Cmd &cmd);
    void reloadDB();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    // QAbstractItemModel interface
public:
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
        const QModelIndex &parent) override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
        const QModelIndex &parent) const override;
};

#endif // CMDLISTMODEL_H
