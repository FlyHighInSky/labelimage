#include "typeeditdialog.h"

TypeEditDialog::TypeEditDialog(QWidget *parent, QString fileName, QTranslator *translator):
    QDialog(parent, Qt::WindowTitleHint | Qt::CustomizeWindowHint),
    _appendButton(new QPushButton(tr("Append"))),
    _deleteButton(new QPushButton(tr("Delete"))),
    _editButton(new QPushButton(tr("Edit"))),
    _typeListWidget(new QListWidget),
    _mainLayout(new QVBoxLayout),
    _hLayout(new QHBoxLayout),
    _buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok)),
    _fileName(fileName),
    _typeNameList(new QStringList),
    _typeNameFileCount(0)
{
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &TypeEditDialog::accept);
    connect(_appendButton, &QPushButton::clicked, this, &TypeEditDialog::clickAppendButton);
    connect(_editButton, &QPushButton::clicked, this, &TypeEditDialog::clickEditButton);
    connect(_deleteButton, &QPushButton::clicked, this, &TypeEditDialog::clickDeleteButton);
    _deleteButton->setDisabled(true);
    connect(_typeListWidget, &QListWidget::currentRowChanged, this, &TypeEditDialog::changeDeleteButtonStatus);

    _hLayout->addWidget(_editButton);
    _hLayout->addWidget(_appendButton);
    _hLayout->addWidget(_deleteButton);

    _mainLayout->addWidget(_typeListWidget);
    _mainLayout->addLayout(_hLayout);
    _mainLayout->addWidget(_buttonBox);
    this->setLayout(_mainLayout);

    this->setWindowTitle(tr("Target Type Editor"));

    this->initNameListFromFile();
    _typeListWidget->insertItems(0, *_typeNameList);
    for (int i=0; i<_typeListWidget->count(); i++) {
        _typeListWidget->item(i)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    if (_typeListWidget->count()>0) {
        _typeListWidget->item(0)->setSelected(true);
        _deleteButton->setEnabled(true);
        _editButton->setEnabled(true);
    } else {
        _deleteButton->setEnabled(false);
        _editButton->setEnabled(false);
    }

    setLayout(_mainLayout);

    qApp->installTranslator(translator);
}

void TypeEditDialog::initNameListFromFile()
{
    QFile file(_fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString s = in.readLine();
        if(!(s.simplified().isEmpty())) {
            _typeNameList->append(s);
        }
    }
    file.close();
    _typeNameFileCount = _typeNameList->count();
}

void TypeEditDialog::clickAppendButton()
{
    int count = _typeListWidget->count();
    if (count <= 0) {
        _deleteButton->setEnabled(true);
        _editButton->setEnabled(true);
    }

    _typeListWidget->insertItem(count, "new");
    _typeListWidget->item(count)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    _typeListWidget->item(count)->setSelected(true);
    _typeListWidget->setCurrentRow(count);
    _typeListWidget->editItem(_typeListWidget->currentItem());
}

void TypeEditDialog::clickEditButton()
{
    _typeListWidget->editItem(_typeListWidget->currentItem());
}

void TypeEditDialog::clickDeleteButton()
{
    QListWidgetItem *item = _typeListWidget->takeItem(_typeListWidget->currentRow());
    delete item;
}

void TypeEditDialog::changeDeleteButtonStatus(int currentRow)
{
    _deleteButton->setDisabled(currentRow<_typeNameFileCount);
}

void TypeEditDialog::accept()
{
    if (_typeListWidget->count()>0) {
        saveNameListToFile();
        this->close();
    } else {
        QMessageBox b;
        b.about(this, tr("Image Labeler"), tr("Please Input Target Type Names!"));
    }
}

void TypeEditDialog::closeEvent(QCloseEvent *)
{
    delete _mainLayout;
    delete _buttonBox;
    delete _appendButton;
    delete _deleteButton;
    delete _editButton;
    delete _typeListWidget;
    delete _typeNameList;
}

void TypeEditDialog::saveNameListToFile()
{
    QFile file(_fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    for (int i=0; i<_typeListWidget->count(); i++) {
        out << _typeListWidget->item(i)->text() + "\n";
    }
    file.close();
}
