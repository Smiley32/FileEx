#include "categorie.h"
#include "ui_categorie.h"
#include "QColorDialog"
#include <QMessageBox>
#include "connection.h"

Categorie::Categorie(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Categorie)
{
    ui->setupUi(this);
    color = Qt::black;
}

Categorie::~Categorie() {
    delete ui;
}

void Categorie::on_buttonColor_clicked() {
    QColor c = QColorDialog::getColor(Qt::black, this);
    ui->labelApercu->setStyleSheet(QString("QLabel { color:%1; }").arg(c.name()));
    color = c;
}

void Categorie::on_buttonBox_rejected() {
    close();
}

void Categorie::on_buttonBox_accepted() {
    if(ui->textCategorie->text().isEmpty()) {
        QMessageBox::information(0, qApp->tr("Impossible de créer la catégorie"),
            qApp->tr("Il faut un nom à la catégorie\n"), QMessageBox::Cancel);
        return;
    }

    if(!addCategorie(ui->textCategorie->text(), color)) {
        QMessageBox::information(0, qApp->tr("Impossible de créer la catégorie"),
            qApp->tr("La création de la catégorie\na échoué\n"), QMessageBox::Cancel);
        return;
    }

    close();
}
