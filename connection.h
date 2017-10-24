#ifndef CONNECTION_H
#define CONNECTION_H

#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>


static bool createConnection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if(!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;

    /* Script de création de la base de données :
create table documents (id INTEGER primary key autoincrement,
                        path varchar(512),
                        ext varchar(10),
                        hash varchar(32)
                       );
create table categories (id INTEGER primary key autoincrement,
                         label varchar(64),
                         color varchar(7)
                        );
create table keywords (id INTEGER primary key autoincrement,
                       label varchar(64),
                       catId INTEGER,
                       foreign key (catId) references categories(id)
                      );
create table docKey (docId INTEGER,
                     keyId INTEGER,
                     primary key (docId, keyId),
                     foreign key (docId) references documents(id),
                     foreign key (keyId) references keywords(id)
                    );
*/


    query.exec("create table documents (id INTEGER primary key, path varchar(512), ext varchar(10), hash varchar(32) );");
    query.exec("create table categories (id INTEGER primary key, label varchar(64), color varchar(6) );");
    query.exec("create table keywords (id INTEGER primary key, label varchar(64), catId INTEGER, foreign key (catId) references categories(id) );");
    query.exec("create table docKey (docId INTEGER, keyId INTEGER, primary key (docId, keyId), foreign key (docId) references documents(id), foreign key (keyId) references keywords(id) );");
    query.exec("INSERT INTO categories (id, label, color) VALUES (0, 'default', '#000000')");
    query.exec("INSERT INTO keywords (id, label, catId) VALUES (0, 'tagme', 0)");

    return true;
}

static bool addCategorie(QString libelle, QColor color) {
    QSqlQuery query;

    query.exec(QString("SELECT * FROM categories WHERE label='%1'").arg(libelle));
    if(query.next()) {
        // On a trouvé une catégorie qui a le même nom
        QMessageBox::critical(0, qApp->tr("Impossible d'ajouter la catégorie"),
            qApp->tr("Une catégorie existe déjà avec ce nom\n"), QMessageBox::Cancel);
        return false;
    }

    return query.exec(QString("insert into categories (label, color) values ('%1', '%2')").arg(libelle, color.name()));
}

static bool addKeyword(QString libelle, int catId) {
    QSqlQuery query;

    query.exec(QString("SELECT * FROM keywords WHERE label='%1'").arg(libelle));
    if(query.next()) {
        // On a trouvé un mot clé qui a le même libellé
        QMessageBox::critical(0, qApp->tr("Impossible d'ajouter le mot clé"),
            qApp->tr("Un mot clé existe déjà avec ce nom\n"), QMessageBox::Cancel);
        return false;
    }

    return query.exec(QString("INSERT INTO keywords (label, catId) VALUES ('%1', %2)").arg(libelle).arg(catId));
}

#endif // CONNECTION_H
