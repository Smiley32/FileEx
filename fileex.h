#ifndef FILEEX_H
#define FILEEX_H

#include <QMainWindow>
#include "connection.h"
#include <QHBoxLayout>
#include <QFileIconProvider>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <windows.h>

namespace Ui {
class FileEx;
}

class FileEx : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileEx(QWidget *parent = 0);
    ~FileEx();

private slots:
    void on_actionAjouter_triggered();

    void on_buttonAjouter_clicked();

    void on_buttonSearch_clicked();

    void on_buttonDelete_clicked();

    void on_listDoc_doubleClicked(const QModelIndex &index);

    void showContextMenu(const QPoint&);

    void eraseItem();
    void addKeyword();
    void deleteDoc();

private:
    Ui::FileEx *ui;
    void updateDocuments(QString sql);
};

QIcon extToIcon(QFileInfo info, QString ext);

#endif // FILEEX_H
