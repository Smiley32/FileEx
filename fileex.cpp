#include "fileex.h"
#include "ui_fileex.h"
#include "ajout.h"
#include "tagedit.h"
#include "ui_ajout.h"
#include <QSqlTableModel>
#include <QCompleter>
#include <QFileSystemModel>
#include <QDesktopServices>

#include <QDebug>

FileEx::FileEx(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileEx)
{
    ui->setupUi(this);

    ui->top->setFixedHeight(50);
    ui->listDoc->setResizeMode(QListWidget::Adjust);

    QSqlTableModel *model = new QSqlTableModel(this, QSqlDatabase::database());
    model->setTable("keywords");
    model->select();

    QCompleter *keywordCompleter = new QCompleter(model, this);
    keywordCompleter->setCompletionColumn(1);
    ui->textSearch->setCompleter(keywordCompleter);

    ui->listDoc->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listDoc, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    updateDocuments("SELECT * FROM documents LIMIT 10");
}

void FileEx::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = ui->listDoc->mapToGlobal(pos);

    // Right clic menu
    QMenu myMenu;
    // myMenu.addAction("Insert", this, SLOT(addItem()));

    myMenu.addAction("Erase",  this, SLOT(eraseItem()));
    myMenu.addAction("Modifier les mots clés", this, SLOT(addKeyword()));
    myMenu.addAction("Supprimer", this, SLOT(deleteDoc()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void FileEx::updateDocuments(QString sql) {
    QSqlQuery query;
    query.exec(sql);

    // Hard link creation
    // CreateHardLinkA("C:\\Users\\Smiley32\\Pictures\\test\\test.jpg", "C:\\Users\\Smiley32\\Pictures\\touhou_medley.jpg", NULL);

    /*QString folderPath = "tmp";

    if(QDir(folderPath).exists()) {
        // Si le dossier existe, on le supprime
        QDir(folderPath).removeRecursively();
    }

    // Création du dossier
    if(!QDir().mkdir(folderPath)) {
        // Erreur de création du répertoire temporaire
        QMessageBox::critical(0, qApp->tr("Impossible de créer le répertoire temporaire"),
            qApp->tr("Avez-vous les permissions dans le dossier de l'exe ?\n"), QMessageBox::Cancel);
        return;
    }*/

    ui->listDoc->clear();

    while(query.next()) {
        QFileInfo info(query.value("path").toString());

        if(info.exists()) {
            // QString filePath = QString("tmp/%1.%2").arg(query.value("id").toString(), info.completeSuffix());
            // qDebug() << filePath << endl;
            // CreateHardLinkA(filePath.toStdString().c_str(), query.value("path").toString().toStdString().c_str(), NULL);

            QListWidgetItem *item = new QListWidgetItem;
            item->setIcon(extToIcon(info, query.value("ext").toString()));
            item->setText(info.baseName());
            item->setData(Qt::UserRole, query.value("id"));
            ui->listDoc->addItem(item);
        }
    }
}

FileEx::~FileEx()
{
    delete ui;
}

void FileEx::on_actionAjouter_triggered() {
    // On affiche la fenêtre d'ajout

    Ajout a;
    a.setWindowTitle("Ajout d'un document");
    a.setModal(true);
    a.exec();
}

QIcon extToIcon(QFileInfo info, QString ext) {
    ext = ext.toUpper();
    if(ext == "JPG" || ext == "JPEG" || ext == "PNG" || ext == "GIF" || ext == "BMP" || ext == "ICO") {
        return QIcon(info.absoluteFilePath());
    }

    if(QFile(QString("D:/Documents/qt/FileEx/icones/%1.ico").arg(ext)).exists()) {
        return QIcon(QString("D:/Documents/qt/FileEx/icones/%1.ico").arg(ext));
    } else {
        QFileIconProvider ip;
        return ip.icon(info);
    }
}

void FileEx::on_buttonAjouter_clicked()
{
    if(ui->textSearch->text().isEmpty()) {
        QMessageBox::information(0, qApp->tr("Impossible d'ajouter le mot clé"),
            qApp->tr("Le champ est vide\n"), QMessageBox::Cancel);
        return;
    }

    ui->listeKeywords->addItem(ui->textSearch->text());
    ui->textSearch->clear();
    ui->textInfo->clear();
}

void FileEx::on_buttonSearch_clicked()
{
    QString sql;
    int count = ui->listeKeywords->count();
    if(count == 0) {
        // Rien à rechercher --> on affiche tout
        // SELECT * FROM documents
        sql = "SELECT * FROM documents LIMIT 10";
    } else {
        sql = "SELECT * FROM documents WHERE ";

        for(int i = 0; i < count; i++) {
            sql += "id IN (SELECT docId FROM keywords, docKey WHERE keyId=id AND label='" + ui->listeKeywords->item(i)->text() + "')";
            if(i != count - 1) {
                sql += " AND ";
            } else {
                sql += " LIMIT 10;";
            }
        }
    }
    qDebug() << "SQL : " << sql << endl;

    updateDocuments(sql);
}

void FileEx::on_buttonDelete_clicked()
{
    ui->listeKeywords->takeItem(ui->listeKeywords->currentRow());
    if(ui->listeKeywords->count() == 0) {
        ui->textInfo->setText("(Aucun mot clé)");
    }
}

void FileEx::on_listDoc_doubleClicked(const QModelIndex &index)
{
    // QFileSystemModel::filePath(index);

    // QDesktopServices::openUrl();

    // QFileSystemModel *model = new QFileSystemModel;

    // qDebug() << "Path : '" << ui->listDoc->item(index.row())->data(Qt::UserRole).toString() << "'" << endl;

    QSqlQuery query;
    query.exec(QString("SELECT * FROM documents WHERE id=%1").arg(ui->listDoc->item(index.row())->data(Qt::UserRole).toInt()));
    if(query.next()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(query.value("path").toString()));
    }
}

// MENU CLIC DROIT

void FileEx::eraseItem()
{
    // If multiple selection is on, we need to erase all selected items
    for (int i = 0; i < ui->listDoc->selectedItems().size(); ++i) {
        // Get curent item on selected row
        QListWidgetItem *item = ui->listDoc->takeItem(ui->listDoc->currentRow());
        // And remove it
        delete item;
    }
}

void FileEx::addKeyword()
{
    // Ajout d'un ou plusieurs mots-clés au document cliqué

    QListWidgetItem *item = ui->listDoc->item(ui->listDoc->currentRow());
    QVariant data = item->data(Qt::UserRole);
    int id = data.toInt();

    qDebug() << id << endl;

    TagEdit t;
    t.setWindowTitle("Modifier les mots clés");
    t.setDocId(id);
    t.setModal(true);
    t.exec();
}

void FileEx::deleteDoc()
{
    // Suppression du document cliqué
    // ..avec affichage d'une confirmation
}
