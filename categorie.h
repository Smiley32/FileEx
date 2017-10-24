#ifndef CATEGORIE_H
#define CATEGORIE_H

#include <QDialog>

namespace Ui {
class Categorie;
}

class Categorie : public QDialog
{
    Q_OBJECT

public:
    explicit Categorie(QWidget *parent = 0);
    ~Categorie();

private slots:
    void on_buttonColor_clicked();

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::Categorie *ui;
    QColor color;
};

#endif // CATEGORIE_H
