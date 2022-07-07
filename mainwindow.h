#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPersistentModelIndex>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onItemButtonClicked(const QModelIndex &index);
    void onSaveButtonClicked();
    void onAction();
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::MainWindow *ui;
    QPersistentModelIndex currentIndex;
    QAction *addAction;
    QAction *removeAction;
    QAction *autoOpenAction;
};
#endif // MAINWINDOW_H
