#ifndef TYPEECITDIALOG_H
#define TYPEECITDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QTranslator>
#include <QApplication>

class TypeEditDialog : public QDialog
{
    Q_OBJECT
public:
    TypeEditDialog(QWidget *parent, QString fileName, QTranslator *translator);
private:
    QListWidget *_typeListWidget;
    QDialogButtonBox *_buttonBox;
    QPushButton *_editButton, *_deleteButton, *_appendButton;
    QVBoxLayout *_mainLayout;
    QHBoxLayout *_hLayout;
    QString _fileName;
    QStringList *_typeNameList;
    int _typeNameFileCount;

    void closeEvent(QCloseEvent *);
    void initNameListFromFile();
    void saveNameListToFile();

private slots:
    void clickEditButton();
    void clickAppendButton();
    void clickDeleteButton();
    void changeDeleteButtonStatus(int currentRow);
    void accept();

};

#endif // TYPEECITDIALOG_H
