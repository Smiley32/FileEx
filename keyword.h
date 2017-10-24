#ifndef KEYWORD_H
#define KEYWORD_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Ui {
class Keyword;
}

class Keyword : public QDialog
{
    Q_OBJECT

public:
    explicit Keyword(QWidget *parent = 0);
    ~Keyword();

private slots:
    void on_buttonCatAjout_clicked();

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::Keyword *ui;
    void updateList();
};

#endif // KEYWORD_H
