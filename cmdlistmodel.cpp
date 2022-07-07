#include "cmdlistmodel.h"

#include <QDir>
#include <QMimeData>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>

CmdListModel::CmdListModel(QObject *parent)
    : QAbstractListModel { parent }
{
    reloadDB();
}

int CmdListModel::rowCount(const QModelIndex &parent) const
{
    return cmdList.count();
}

QVariant CmdListModel::data(const QModelIndex &index, int role) const
{
    const Cmd &cmd = cmdList[index.row()];
    if (role == Qt::DisplayRole) {
        return cmd.name;
    }
    if (role == StateRole) {
        return procMap.contains(cmd.id);
    }
    if (role == DataRole) {
        return QVariant::fromValue(cmd);
    }
    if (role == FileRole) {
        static QRegularExpression sReg("[^\\d\\w]");
        return QDir::temp().filePath(QString(cmd.name).replace(sReg, "_"));
    }
    return QVariant();
}

bool CmdListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "setData " << index << " " << value << " " << role;
    if (role == StateRole) {
        toggleProc(index);
        return true;
    }
    if (role == DataRole) {
        const Cmd &cmd = value.value<Cmd>();
        cmdList.replace(index.row(), cmd);
        updateDB(cmd);
        emit dataChanged(this->index(index.row()), this->index(index.row()), { Qt::DisplayRole });
        return true;
    }
    return false;
}

Qt::ItemFlags CmdListModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool CmdListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "insertRows " << row << " " << count;
    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r) {
        Cmd cmd;
        insertDB(cmd);
        cmdList.insert(row, cmd);
    }
    assignPositions();

    endInsertRows();
    return true;
}

bool CmdListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "removeRows " << row << " " << count;
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int r = row; r < row + count; ++r) {
        Cmd cmd = cmdList.takeAt(r);
        qDebug() << "removeRows " << cmd << cmdList.size();
        deleteDB(cmd);
        if (procMap.contains(cmd.id)) {
            procMap.value(cmd.id)->kill();
        }
    }

    endRemoveRows();
    return true;
}

Qt::DropActions CmdListModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

QStringList CmdListModel::mimeTypes() const
{
    QStringList types;
    types << QStringLiteral("application/x-gloggcat-cmd-row");
    return types;
}

QMimeData *CmdListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QDataStream::WriteOnly);
    const Cmd &cmd = indexes.at(0).data(DataRole).value<Cmd>();
    stream << indexes.at(0).row();
    data->setData(mimeTypes().at(0), encoded);
    return data;
}

bool CmdListModel::dropMimeData(
    const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (row < 0) {
        return false;
    }
    QByteArray encoded = data->data(mimeTypes().at(0));
    QDataStream stream(&encoded, QDataStream::ReadOnly);
    int sourceRow;
    stream >> sourceRow;
    int targetRow = row > sourceRow ? row - 1 : row;
    const Cmd &source = cmdList.takeAt(sourceRow);
    cmdList.insert(targetRow, source);
    assignPositions();
    return true;
}

bool CmdListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
    int column, const QModelIndex &parent) const
{
    return data->hasFormat(mimeTypes().at(0));
}

void CmdListModel::assignPositions()
{
    for (int i = 0; i < cmdList.size(); i++) {
        Cmd &cmd = cmdList[i];
        cmd.pos = i;
        updateDB(cmd);
    }
}

void CmdListModel::reloadDB()
{
    cmdList.clear();
    QSqlQuery query("SELECT * FROM cmd ORDER BY pos ASC");
    while (query.next()) {
        Cmd cmd;
        cmd.id = query.value(0).toUInt();
        cmd.name = query.value(1).toString();
        cmd.exec = query.value(2).toString();
        cmd.pos = query.value(3).toUInt();
        cmdList.append(cmd);
    }
}

void CmdListModel::updateDB(const Cmd &cmd)
{
    QSqlQuery query;
    query.prepare("UPDATE cmd SET name=:name, exec=:exec, pos=:pos WHERE id=:id");
    query.bindValue(":name", cmd.name);
    query.bindValue(":exec", cmd.exec);
    query.bindValue(":pos", cmd.pos);
    query.bindValue(":id", cmd.id);
    query.exec();
}

void CmdListModel::insertDB(Cmd &cmd)
{
    QSqlQuery query;
    query.prepare("INSERT INTO cmd DEFAULT VALUES");
    query.exec();
    cmd.id = query.lastInsertId().toUInt();
}

void CmdListModel::deleteDB(const Cmd &cmd)
{
    QSqlQuery query;
    query.prepare("DELETE FROM cmd WHERE id=:id");
    query.bindValue(":id", cmd.id);
    query.exec();
}

void CmdListModel::toggleProc(const QModelIndex &index)
{
    const Cmd &cmd = index.data(DataRole).value<Cmd>();
    if (cmd.exec.isEmpty()) {
        return;
    }
    QProcess *proc = procMap.value(cmd.id, nullptr);
    if (proc == nullptr) {
        proc = new QProcess(this);
        procMap[cmd.id] = proc;
        connect(proc, &QProcess::stateChanged, this, &CmdListModel::procStateChanged);
        proc->setStandardOutputFile(index.data(FileRole).toString());
        proc->startCommand(cmd.exec);
    } else {
        proc->kill();
    }
}

void CmdListModel::procStateChanged(QProcess::ProcessState state)
{
    QProcess *proc = qobject_cast<QProcess *>(sender());
    int id = procMap.key(proc);
    if (state == QProcess::NotRunning) {
        procMap.remove(id);
        proc->deleteLater();
    }

    int row = -1;
    for (int i = 0; i < cmdList.count(); ++i) {
        if (cmdList[i].id == id) {
            row = i;
            break;
        }
    }
    if (row == -1) {
        return;
    }
    emit dataChanged(index(row), index(row));
}

QDebug operator<<(QDebug dbg, const Cmd &c)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "Cmd(" << c.id << ", " << c.name << ", " << c.pos << ")";
    return dbg;
}

Cmd::~Cmd()
{
}
