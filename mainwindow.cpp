#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "cmdlistdelegate.h"
#include "cmdlistmodel.h"
#include <QScreen>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    addAction = new QAction(QIcon("://res/list-add.png"), "Add", this);
    removeAction = new QAction(QIcon(":/res/list-remove.png"), "Remove", this);
    autoOpenAction = new QAction(QIcon(":/res/auto-open.svg"), "Auto open glogg", this);
    autoOpenAction->setCheckable(true);
    autoOpenAction->setChecked(QSettings().value("auto_open", true).toBool());
    ui->toolBar->addAction(addAction);
    ui->toolBar->addAction(removeAction);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(autoOpenAction);
    connect(addAction, &QAction::triggered, this, &MainWindow::onAction);
    connect(removeAction, &QAction::triggered, this, &MainWindow::onAction);
    connect(autoOpenAction, &QAction::toggled, this, &MainWindow::onAction);

    CmdListModel *model = new CmdListModel(ui->listView);
    CmdListDelegate *delegate = new CmdListDelegate(ui->listView);
    ui->listView->setModel(model);
    ui->listView->setItemDelegate(delegate);
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::onItemDoubleClicked);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this,
        &MainWindow::currentChanged);
    connect(delegate, &CmdListDelegate::buttonClicked, this, &MainWindow::onItemButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onItemDoubleClicked(const QModelIndex &index)
{
    const QString &file = index.data(FileRole).toString();
    QProcess process;
    process.startDetached("glogg", { file });
}

void MainWindow::onItemButtonClicked(const QModelIndex &index)
{
    ui->listView->model()->setData(index, QVariant(), StateRole);
}

void MainWindow::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    currentIndex = current;
    const Cmd &cmd = current.data(DataRole).value<Cmd>();
    ui->nameLineEdit->setText(cmd.name);
    ui->commandLineEdit->setText(cmd.exec);
}

void MainWindow::onSaveButtonClicked()
{
    Cmd cmd = currentIndex.data(DataRole).value<Cmd>();
    cmd.name = ui->nameLineEdit->text();
    cmd.exec = ui->commandLineEdit->text();
    ui->listView->model()->setData(currentIndex, QVariant::fromValue(cmd), DataRole);
}

void MainWindow::onAction()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action == addAction) {
        ui->listView->model()->insertRow(currentIndex.row() + 1);
    } else if (action == removeAction) {
        ui->listView->model()->removeRow(currentIndex.row());
    } else if (action == autoOpenAction) {
        QSettings().setValue("auto_open", action->isChecked());
    }
}
