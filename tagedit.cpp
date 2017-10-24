#include "tagedit.h"
#include "ui_tagedit.h"
#include "connection.h"
#include "keyword.h"
#include <QMessageBox>

TagEdit::TagEdit(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::TagEdit)
{
    ui->setupUi(this);
}

TagEdit::~TagEdit()
{
    delete ui;
}

void TagEdit::setDocId(int docId)
{
    id = docId;
    updateListes();
}

void TagEdit::updateListes()
{
    ui->listAll->clear();
    ui->listDoc->clear();

    QSqlQuery query;
    query.exec(QString("SELECT * FROM keywords WHERE id NOT IN (SELECT id FROM keywords, docKey WHERE keyId=id AND docId=%1)").arg(id));
    while(query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value("label").toString());
        item->setData(Qt::UserRole, query.value("id"));
        ui->listAll->addItem(item);
    }

    query.exec(QString("SELECT * FROM keywords, docKey WHERE keyId=id AND docId=%1").arg(id));
    while(query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value("label").toString());
        item->setData(Qt::UserRole, query.value("id"));
        ui->listDoc->addItem(item);
    }

    // Désactivation potentielle du bouton 'ajouter'
    if(ui->listAll->count() == 0) {
        ui->buttonAjouter->setEnabled(false);
    }

    // Désactivation potentielle du bouton 'supprimer'
    if(ui->listDoc->count() == 0) {
        ui->buttonSupprimer->setEnabled(false);
    }
}

void TagEdit::on_buttonNouveau_clicked() {
    // On affiche la fenêtre d'ajout de mot clé

    Keyword k;
    k.setWindowTitle("Ajout de mot clé");
    k.setModal(true);
    k.exec();

    // Après l'ajout potentiel de mot clé
    updateListes();
}

void TagEdit::on_buttonBox_rejected() {
    close();
}

void TagEdit::on_buttonAjouter_clicked()
{
    // On bascule l'item selectionné dans la seconde liste
    if(ui->listAll->count() != 0) {
        ui->listDoc->addItem(ui->listAll->takeItem(ui->listAll->currentRow()));
    }

    if(ui->listAll->count() == 0) {
        ui->buttonAjouter->setEnabled(false);
    }

    if(ui->listDoc->count() != 0) {
        ui->buttonSupprimer->setEnabled(true);
    }
}

void TagEdit::on_buttonSupprimer_clicked()
{
    if(ui->listDoc->count() != 0) {
        ui->listAll->addItem(ui->listDoc->takeItem(ui->listDoc->currentRow()));
    }

    if(ui->listAll->count() != 0) {
        ui->buttonAjouter->setEnabled(true);
    }

    if(ui->listDoc->count() == 0) {
        ui->buttonSupprimer->setEnabled(false);
    }
}

void TagEdit::on_buttonBox_accepted()
{
    if(ui->listDoc->count() == 0) {
        // Il faut définir un mot clé
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Vous devez choisir au moins un mot clé\n"), QMessageBox::Cancel);
        return;
    }

    int nbKeywords = ui->listDoc->count();

    QSqlQuery query;

    // On commence par supprimer les mots clés qu'il y avait avant
    query.exec(QString("DELETE FROM docKey WHERE docId=%1").arg(id));

    for(int i = 0; i < nbKeywords; i++) {
        qDebug() << i << "/" << nbKeywords << endl;
        QVariant v = ui->listDoc->item(i)->data(Qt::UserRole);
        int keywordId = v.value<int>();

        // qDebug() << QString("INSERT INTO docKey (docId, keyId) VALUES (%1, %2)").arg(lastInsertId).arg(keywordId) << endl;
        query.exec(QString("INSERT INTO docKey (docId, keyId) VALUES (%1, %2)").arg(id).arg(keywordId));
    }

    close();
}
