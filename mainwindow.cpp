
#include <QScrollBar>
#include <QImageWriter>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <functional>
#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#endif

#include "mainwindow.h"

MainWindow::MainWindow()
{
    setWindowIcon(QIcon(":/images/draw.png"));
    setWindowTitle(tr("Image Labeler"));
    createActions();
    createCentralWindow();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    this->installEventFilter(this);
    fileListView->installEventFilter(this);
    imageView->installEventFilter(this);

    _languageFile = ":/languages/zh_CN.qm";
    translator.load(_languageFile);
    qApp->installTranslator( &translator );
    this->retranslate();
}
/**
 * @brief MainWindow::openDirectory
 *        Open the specified directory that contains images to be labeled.
 */
void MainWindow::openFolder()
{
    QString srcImageDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                            "",
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
    if (!srcImageDir.isEmpty()) {
        setWindowTitle(srcImageDir);

        if (fileListModel){
            delete fileListModel;
            fileListModel = nullptr;
            editAct->setEnabled(false);
        }

        // init dirmodel
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.tif" << "*.tiff" << "*.bmp";
        fileListModel = new QDirModel(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name, this);

        // init treeview and set model
//        fileListView->setFocus();
        fileListView->setRootIsDecorated(0);
        fileListView->setModel(fileListModel);
        fileListView->setRootIndex(fileListModel->index(srcImageDir));
        fileListView->setSelectionMode(QAbstractItemView::SingleSelection);

        connect(fileListView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::onFileSelected);

        // load typenamefile and init typename list.
        _typeNameList.clear();
        _typeNameFile= srcImageDir + "/names.txt";
        _typeNameList = loadTypeNameFromFile(_typeNameFile);
        editAct->setEnabled(true);
        if (_typeNameList.count() <= 0) {
            editTargetType();
            _typeNameList = loadTypeNameFromFile(_typeNameFile);
        }

        // set the first image selected.
        QModelIndex index = fileListModel->index(0, 0, fileListView->rootIndex());
        fileListView->setCurrentIndex(index);
    }
}

QStringList MainWindow::loadTypeNameFromFile(QString filePath)
{
    QStringList typeNameList;
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString s = in.readLine();
        if(!(s.simplified().isEmpty())) {
            typeNameList.append(s);
        }
    }
    file.close();

    return typeNameList;
}

void MainWindow::help()
{
    _helpMessageBox.about(this,
                          qApp->translate("MainWindow", "Help"),
                          qApp->translate("MainWindow", helpText));
}

void MainWindow::about()
{
    _aboutMessageBox.about(this,
                           qApp->translate("MainWindow", "About Image Labeler"),
                           qApp->translate("MainWindow", aboutText));
}

/**
 * @brief MainWindow::createActions add menu actions
 */
void MainWindow::createActions()
{
    // file menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileToolBar = addToolBar(tr("File"));

    // open folder
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/folder.png"));
    openAct = new QAction(openIcon, tr("&Open Folder..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an image folder"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFolder);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // quit
    const QIcon exitIcon = QIcon::fromTheme("application-exit", QIcon(":/images/quit.png"));
    exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
    fileToolBar->addAction(exitAct);

    menuBar()->addSeparator();

    // edit menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editToolBar = addToolBar(tr("Edit"));

    // edit target type
    editAct = new QAction(QIcon(":/images/edit.png"), tr("&Target Type"), this);
    editAct->setShortcut(tr("Ctrl+T"));
    editAct->setStatusTip(tr("Edit Target Type"));
    connect(editAct, &QAction::triggered, this, &MainWindow::editTargetType);
    editMenu->addAction(editAct);
    editToolBar->addAction(editAct);
    editAct->setEnabled(false);

    _undoGroup = new QUndoGroup(this);
    // undo
    undoAct = _undoGroup->createUndoAction(this);
    undoAct->setIcon(QIcon(":/images/undo.png"));
    undoAct->setText(tr("&Undo"));
    undoAct->setStatusTip(tr("Undo"));
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    // redo
    redoAct = _undoGroup->createRedoAction(this);
    redoAct->setIcon(QIcon(":/images/redo.png"));
    redoAct->setText(tr("&Redo"));
    redoAct->setStatusTip(tr("Redo"));
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    menuBar()->addSeparator();

    // view menu
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewToolBar = addToolBar(tr("View"));

    // zoom in
    const QIcon zoomInIcon = QIcon::fromTheme("document-open", QIcon(":/images/zoom-in.png"));
    zoomInAct = new QAction(zoomInIcon, tr("Zoom &In"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setStatusTip(tr("Zoom In Image"));
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);
    viewMenu->addAction(zoomInAct);
    viewToolBar->addAction(zoomInAct);

    // zoom out
    const QIcon zoomOutIcon = QIcon::fromTheme("document-open", QIcon(":/images/zoom-out.png"));
    zoomOutAct = new QAction(zoomOutIcon, tr("Zoom &Out"), this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setStatusTip(tr("Zoom Out Image"));
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);
    viewMenu->addAction(zoomOutAct);
    viewToolBar->addAction(zoomOutAct);

    // fit to window
    const QIcon fitInIcon = QIcon::fromTheme("document-open", QIcon(":/images/fit-to-window.png"));
    fitToWindowAct = new QAction(fitInIcon, tr("&Fit To Window"), this);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    fitToWindowAct->setStatusTip(tr("Fit View To Window"));
    connect(fitToWindowAct, &QAction::triggered, this, &MainWindow::fitViewToWindow);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setChecked(true);
    viewMenu->addAction(fitToWindowAct);
    viewToolBar->addAction(fitToWindowAct);

    // actual size
    const QIcon actualSizeIcon = QIcon::fromTheme("document-open", QIcon(":/images/actual-size.png"));
    actualSizeAct = new QAction(actualSizeIcon, tr("&Actual Size"), this);
    actualSizeAct->setShortcut(tr("Ctrl+R"));
    actualSizeAct->setStatusTip(tr("Fit View To Actual Size"));
    connect(actualSizeAct, &QAction::triggered, this, &MainWindow::fitViewToActual);
    viewMenu->addAction(actualSizeAct);
    viewToolBar->addAction(actualSizeAct);

    viewMenu->addSeparator();

    // full screen
    const QIcon fullscreenIcon = QIcon::fromTheme("document-open", QIcon(":/images/fullscreen.png"));
    fullscreenAct = new QAction(fullscreenIcon, tr("&Full Screen"), this);
    fullscreenAct->setShortcut(tr("Alt+Enter"));
    fullscreenAct->setStatusTip(tr("Full Screen"));
    connect(fullscreenAct, &QAction::triggered, this, &MainWindow::fullScreen);
    viewMenu->addAction(fullscreenAct);
    viewToolBar->addAction(fullscreenAct);

    // help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpToolBar = addToolBar(tr("Help"));

    // language
    languageMenu = helpMenu->addMenu(
                QIcon::fromTheme("document-open", QIcon(":/images/language.png")), tr("&Language"));
    languageMenu->setStatusTip(tr("Select language"));

    // Chinese
    zhCNAct = languageMenu->addAction(
                QIcon::fromTheme("document-open", QIcon(":/images/zh_CN.png")),
                tr("&Chinese"));
    languageMenu->addAction(zhCNAct);
    helpToolBar->addAction(zhCNAct);
    connect(zhCNAct, &QAction::triggered, this, &MainWindow::switchLanguage);

    // English
    enUSAct = languageMenu->addAction(
                QIcon::fromTheme("document-open", QIcon(":/images/en_US.png")),
                tr("&English"));
    languageMenu->addAction(enUSAct);
    helpToolBar->addAction(enUSAct);
    connect(enUSAct, &QAction::triggered, this, &MainWindow::switchLanguage);

    // help
    helpAct = helpMenu->addAction(
                QIcon::fromTheme("document-open", QIcon(":/images/help.png")),
                tr("&Help"), this, &MainWindow::help);
    helpAct->setStatusTip(tr("Help"));
    helpAct->setShortcut(QKeySequence::HelpContents);
    helpToolBar->addAction(helpAct);

    // about
    aboutAct = helpMenu->addAction(
                QIcon::fromTheme("document-open", QIcon(":/images/about.png")),
                tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("About Image Labeler"));
    helpToolBar->addAction(aboutAct);
}

void MainWindow::switchLanguage()
{
    if (this->sender() == zhCNAct) {
        _languageFile = ":/languages/zh_CN.qm";
    } else if (this->sender() == enUSAct) {
        _languageFile = ":/languages/en_US.qm";
    }
    translator.load(_languageFile);
    qApp->installTranslator( &translator );
    this->retranslate();
}

void MainWindow::retranslate()
{
    // file menu
    fileMenu->setTitle(tr("&File"));
    //    fileToolBar->setTitle(tr("File"));

    // open folder
    openAct->setText(tr("&Open Folder..."));
    openAct->setStatusTip(tr("Open an image folder"));

    // quit
    exitAct->setText(tr("E&xit"));
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));

    // edit menu
    editMenu->setTitle(tr("&Edit"));

    // edit target type
    editAct ->setText(tr("&Target Type"));
    editAct->setShortcut(tr("Ctrl+T"));
    editAct->setStatusTip(tr("Edit Target Type"));

    // undo
    undoAct->setText(tr("&Undo"));
    undoAct->setStatusTip(tr("Undo"));

    // redo
    redoAct->setText(tr("&Redo"));
    redoAct->setStatusTip(tr("Redo"));

    // view menu
    viewMenu->setTitle(tr("&View"));
    //    viewToolBar->setTitle(tr("View"));

    // zoom in
    zoomInAct->setText(tr("Zoom &In"));
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setStatusTip(tr("Zoom In Image"));
    // zoom out
    zoomOutAct->setText(tr("Zoom &Out"));
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setStatusTip(tr("Zoom Out Image"));
    // fit to window
    fitToWindowAct->setText(tr("&Fit To Window"));
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    fitToWindowAct->setStatusTip(tr("Fit View To Window"));

    // actual size
    actualSizeAct->setText(tr("&Actual Size"));
    actualSizeAct->setShortcut(tr("Ctrl+R"));
    actualSizeAct->setStatusTip(tr("Fit View To Actual Size"));

    // full screen
    fullscreenAct->setText(tr("&Full Screen"));
    fullscreenAct->setShortcut(tr("Alt+Enter"));
    fullscreenAct->setStatusTip(tr("Full Screen"));
    // help menu
    helpMenu->setTitle(tr("&Help"));
    //    helpToolBar = addToolBar(tr("Help"));

    // language
    languageMenu->setTitle(tr("&Language"));
    languageMenu->setStatusTip(tr("Select language"));

    // Chinese
    zhCNAct->setText(tr("&Chinese"));

    // English
    enUSAct->setText(tr("&English"));

    // help
    helpAct->setText(tr("&Help"));
    helpAct->setStatusTip(tr("Help"));

    // about
    aboutAct->setText(tr("&About"));
    aboutAct->setToolTip(tr("About Image Labeler"));

    // status bar
    if (!_imageSize.isEmpty())
        _labelImageSize->setText(QString(tr("Image: %1 x %2"))
                                 .arg(_imageSize.width())
                                 .arg(_imageSize.height())
                                 .toUtf8());
    if (!_boxRect.isNull())
        _labelBoxRect->setText(QString(tr("Box: x-%1, y-%2, w-%3, h-%4"))
                               .arg(_boxRect.left())
                               .arg(_boxRect.top())
                               .arg(_boxRect.width())
                               .arg(_boxRect.height())
                               .toUtf8());
    if (!_cursorPos.isNull())
        _labelCursorPos->setText(QString(tr("Cursor: %1, %2"))
                                 .arg(_cursorPos.x())
                                 .arg(_cursorPos.y())
                                 .toUtf8());
}

void MainWindow::createCentralWindow()
{
    centralWidget = new QWidget(this);
    horizontalLayout = new QHBoxLayout(centralWidget);
    fileListView = new QTreeView(this);
    imageView = new QGraphicsView(this);
    imageView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);


    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainSplitter->addWidget(fileListView);
    mainSplitter->addWidget(imageView);
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 8);
    mainSplitter->setOpaqueResize(false);

    horizontalLayout->addWidget(mainSplitter);
    centralWidget->setLayout(horizontalLayout);
    this->setCentralWidget(centralWidget);

    _labelCursorPos = new QLabel();
    _labelImageSize = new QLabel();
    _labelBoxRect = new QLabel();
    this->statusBar()->addPermanentWidget(new QLabel(), 1);
    this->statusBar()->addPermanentWidget(_labelCursorPos, 1);
    this->statusBar()->addPermanentWidget(_labelBoxRect, 1);
    this->statusBar()->addPermanentWidget(_labelImageSize, 1);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_viewScene) {
        delete _viewScene;
    }
}

void MainWindow::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::onFileSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
    displayImageView(fileListModel->filePath(selected.indexes().first()));
}

void MainWindow::displayImageView(QString imageFilePath)
{
    if (_viewScene) {
        delete _viewScene;
    }
    _viewScene = new ViewScene(this);
    _viewScene->setTypeNameList(_typeNameList);

    _viewScene->installEventFilter(this);
    connect(_viewScene, SIGNAL(cursorMoved(QPointF)), this, SLOT(updateLabelCursorPos(QPointF)));
    connect(_viewScene, SIGNAL(boxSelected(QRect)), this, SLOT(updateLabelBoxRect(QRect)));
    connect(_viewScene, SIGNAL(imageLoaded(QSize)), this, SLOT(updateLabelImageSize(QSize)));

    _viewScene->loadImage(imageFilePath);
    isImageLoaded = true;

    _undoGroup->addStack(_viewScene->undoStack());
    _undoGroup->setActiveStack(_viewScene ? _viewScene->undoStack() : 0);

    imageView->setScene(_viewScene);

    fitToWindowAct->setChecked(true);
    fitViewToWindow();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    fitViewToWindow();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == _viewScene || obj == this) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
                qApp->sendEvent(fileListView, event);
                return true;
            }
            return false;
        } else {
            return false;
        }
    } else {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }
}

void MainWindow::editTargetType()
{
    TypeEditDialog* d = new TypeEditDialog(this, _typeNameFile, &translator);
    int resutl = d->exec();
    if (resutl == QDialog::Accepted) {
        delete d;
    }
}

void MainWindow::zoomIn()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    _viewScene->setViewZoom(_viewScene->viewZoom() * 1.2);
}

void MainWindow::zoomOut()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    _viewScene->setViewZoom(_viewScene->viewZoom() * 0.8);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    auto oldZoom = _viewScene->viewZoom();
    auto newZoom = oldZoom + (event->delta() / 120.0) * 0.05;
    _viewScene->setViewZoom(newZoom <= 0 ? oldZoom : newZoom);
}

void MainWindow::fitViewToWindow()
{
    if (!isImageLoaded)
        return;
    if (!fitToWindowAct->isChecked())
        return;

    _viewScene->setViewZoom(imageView->width() - 2, imageView->height() - 2);
}

void MainWindow::fitViewToActual()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    _viewScene->setViewZoom(1);
}

void MainWindow::fullScreen()
{
    auto makeFullscreen = !isFullScreen();

    fileListView->setVisible(!makeFullscreen);
    setWindowState(makeFullscreen ? Qt::WindowFullScreen : Qt::WindowMaximized);
}

void MainWindow::updateLabelImageSize(QSize imageSize)
{
    _imageSize = imageSize;
    _labelImageSize->setText(QString(tr("Image: %1 x %2"))
                             .arg(_imageSize.width())
                             .arg(_imageSize.height())
                             .toUtf8());
}

void MainWindow::updateLabelBoxRect(QRect boxRect)
{
    _boxRect = boxRect;
    _labelBoxRect->setText(QString(tr("Box: x-%1, y-%2, w-%3, h-%4"))
                           .arg(_boxRect.left())
                           .arg(_boxRect.top())
                           .arg(_boxRect.width())
                           .arg(_boxRect.height())
                           .toUtf8());
}

void MainWindow::updateLabelCursorPos(QPointF cursorPos)
{
    _cursorPos = cursorPos;
    _labelCursorPos->setText(QString(tr("Cursor: %1, %2"))
                             .arg(_cursorPos.x())
                             .arg(_cursorPos.y())
                             .toUtf8());
}
