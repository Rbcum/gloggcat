// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDir>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

/*
    This file defines a helper function to open a connection to an
    in-memory SQLITE database and to create a test table.

    If you want to use another database, simply modify the code
    below. All the examples in this directory use this function to
    connect to a database.
*/
//! [0]
static bool createConnection()
{
    QDir dataDir { QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) };
    dataDir.mkpath(dataDir.absolutePath());
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataDir.filePath("app.db"));
    //    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."),
            QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS cmd (id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT,"
               "exec TEXT,"
               "pos INTEGER)");
    // clang-format off
//        query.exec("INSERT INTO cmd VALUES(0, 'Main log','/data/tools/android-sdk-linux_x86/platform-tools/adb logcat', 1)");
//        query.exec("INSERT INTO cmd VALUES(1, 'Radio log', '/data/tools/android-sdk-linux_x86/platform-tools/adb logcat -b radio', 2)");
//        query.exec("INSERT INTO cmd VALUES(2, 'Dmesg','/data/tools/android-sdk-linux_x86/platform-tools/adb shell dmesg -w', 3)");
//        query.exec("INSERT INTO cmd VALUES(100, 'Sleep 1 sec', 'sleep 1', 100)");
    // clang-format on
    return true;
}
//! [0]

#endif
