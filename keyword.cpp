#include "keyword.h"
#include "ui_keyword.h"
#include "categorie.h"
#include <QMessageBox>
#include "connection.h"
#include <QDebug>
#include "ui_ajout.h"

Keyword::Keyword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Keyword)
{
    ui->setupUi(this);
    this->setFixedSize(size());

    updateList();
}

void Keyword::updateList() {
    ui->listCategories->clear();
    QSqlQuery query;
    query.exec("SELECT label, id FROM categories");
    while(query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value(0).toString());
        item->setData(Qt::UserRole, query.value("id"));
        // ui->listCategories->addItem(query.value(0).toString());
        ui->listCategories->addItem(item);
    }
    ui->listCategories->setCurrentRow(0);
}

Keyword::~Keyword()
{
    delete ui;
}

void Keyword::on_buttonCatAjout_clicked() {
    // On affiche la fenêtre d'ajout de catégorie

    Categorie c;
    c.setWindowTitle("Ajout d'une catégorie");
    c.setModal(true);
    c.exec();

    updateList();
}

void Keyword::on_buttonBox_rejected() {
    close();
}

void Keyword::on_buttonBox_accepted() {
    if(ui->textKeyword->text().isEmpty()) {
        QMessageBox::information(0, qApp->tr("Impossible de créer le mot clé"),
            qApp->tr("Il faut donner un nom au mot clé\n"), QMessageBox::Cancel);
        return;
    }

    QString libelle = ui->textKeyword->text();
    QVariant v = ui->listCategories->currentItem()->data(Qt::UserRole);
    int id = v.value<int>();

    if(!addKeyword(libelle, id)) {
        QMessageBox::information(0, qApp->tr("Impossible de créer le mot clé"),
            qApp->tr("Une erreur est survenue\n"), QMessageBox::Cancel);
        return;
    }

    close();
}
