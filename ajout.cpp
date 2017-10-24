#include "ajout.h"
#include "ui_ajout.h"
#include "keyword.h"
#include "ui_keyword.h"
#include "connection.h"
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDebug>

Ajout::Ajout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ajout)
{
    ui->setupUi(this);
    ui->groupKeywords->hide();

    updateListes();
}

void Ajout::updateListes() {
    ui->listAll->clear();
    ui->listDoc->clear();

    QSqlQuery query;
    query.exec("SELECT * FROM keywords");
    while(query.next()) {
        QListWidgetItem *item = new QListWidgetItem(query.value("label").toString());
        item->setData(Qt::UserRole, query.value("id"));
        ui->listAll->addItem(item);
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

Ajout::~Ajout()
{
    delete ui;
}

void Ajout::on_buttonParcourir_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("All files (*)"));
    if(!fileName.isEmpty()) {
        ui->textFile->setText(fileName);
        ui->groupKeywords->show();
    } else {
        ui->textFile->setText("Aucun fichier n'a été choisi");
        ui->groupKeywords->hide();
    }
}

void Ajout::on_buttonNouveau_clicked() {
    // On affiche la fenêtre d'ajout de mot clé

    Keyword k;
    k.setWindowTitle("Ajout de mot clé");
    k.setModal(true);
    k.exec();

    // Après l'ajout potentiel de mot clé
    updateListes();
}

void Ajout::on_buttonBox_rejected() {
    close();
}

void Ajout::on_buttonAjouter_clicked()
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

void Ajout::on_buttonSupprimer_clicked()
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

void Ajout::on_buttonBox_accepted()
{
    if(ui->listDoc->count() == 0) {
        // Il faut définir un mot clé
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Vous devez choisir au moins un mot clé\n"), QMessageBox::Cancel);
        return;
    }

    if(!QFile(ui->textFile->text()).exists()) {
        // Le fichier n'existe pas
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Le fichier choisi n'existe pas\n"), QMessageBox::Cancel);
        return;
    }

    QFileInfo fileInfo(ui->textFile->text());
    QString ext = fileInfo.suffix();
    QString path = ui->textFile->text();

    QFile file(ui->textFile->text());
    QString md5;
    if(file.open(QIODevice::ReadOnly)) {
        QCryptographicHash crypto(QCryptographicHash::Md5);
        while(!file.atEnd()) {
            crypto.addData(file.read(8192));
        }
        QByteArray hashData = crypto.result();
        md5 = hashData.toHex();
    } else {
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Impossible d'accéder au fichier choisi\n"), QMessageBox::Cancel);
        return;
    }

    // qDebug() << "Infos : " << path << " ; " << ext << " ; " << md5 << endl;
    int nbKeywords = ui->listDoc->count();

    if(nbKeywords == 0) {
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Il faut que vous choisissiez au moins un mot clé\n"), QMessageBox::Cancel);
        return;
    }

    QSqlQuery query;
    qDebug() << QString("INSERT INTO documents (path, ext, hash) VALUES ('%1', '%2', '%3')").arg(path, ext, md5) << endl;

    bool ret = query.exec(QString("INSERT INTO documents (path, ext, hash) VALUES ('%1', '%2', '%3')").arg(path, ext, md5));
    if(!ret) {
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le fichier"),
            qApp->tr("Une erreur est survenue\n"), QMessageBox::Cancel);
        return;
    }

    int lastInsertId = query.lastInsertId().toInt();
    for(int i = 0; i < nbKeywords; i++) {
        qDebug() << i << "/" << nbKeywords << endl;
        QVariant v = ui->listDoc->item(i)->data(Qt::UserRole);
        int keywordId = v.value<int>();

        qDebug() << QString("INSERT INTO docKey (docId, keyId) VALUES (%1, %2)").arg(lastInsertId).arg(keywordId) << endl;
        query.exec(QString("INSERT INTO docKey (docId, keyId) VALUES (%1, %2)").arg(lastInsertId).arg(keywordId));
    }

    close();
}
