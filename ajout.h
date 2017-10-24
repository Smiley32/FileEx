#ifndef AJOUT_H
#define AJOUT_H

#include <QDialog>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Ui {
class Ajout;
}

class Ajout : public QDialog
{
    Q_OBJECT

public:
    explicit Ajout(QWidget *parent = 0);
    ~Ajout();

private slots:
    void on_buttonParcourir_clicked();
    void on_buttonNouveau_clicked();
    void on_buttonBox_rejected();
    void on_buttonAjouter_clicked();
    void on_buttonSupprimer_clicked();
    void on_buttonBox_accepted();

private:
    void updateListes();
    Ui::Ajout *ui;
};

#endif // AJOUT_H
