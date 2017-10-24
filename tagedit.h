#ifndef TAGEDIT_H
#define TAGEDIT_H

#include <QDialog>

namespace Ui {
class TagEdit;
}

class TagEdit : public QDialog
{
    Q_OBJECT

public:
    explicit TagEdit(QDialog *parent = 0);
    void setDocId(int docId);
    ~TagEdit();

private slots:
    void on_buttonNouveau_clicked();
    void on_buttonBox_rejected();
    void on_buttonAjouter_clicked();
    void on_buttonSupprimer_clicked();
    void on_buttonBox_accepted();

private:
    Ui::TagEdit *ui;
    int id = -1;
    void updateListes();
};

#endif // TAGEDIT_H
