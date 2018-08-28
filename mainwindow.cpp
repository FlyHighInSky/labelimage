
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
    createActions();
    createCentralWindow();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    this->installEventFilter(this);
    fileListView->installEventFilter(this);
    imageView->installEventFilter(this);
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
        fileListModel->setFilter(QDir::Files | QDir::NoDotAndDotDot);
        fileListModel->setNameFilters(filters);
        fileListModel->setNameFilterDisables(0);
        fileListModel->setRootPath(srcImageDir);

        fileListView->setFocus();
        fileListView->setRootIsDecorated(0);
        fileListView->setModel(fileListModel);
        fileListView->setRootIndex(fileListModel->index(srcImageDir));

        //        fileListView->setSelectionBehavior(QAbstractItemView::SelectRows);
        //        fileListView->setCurrentIndex(fileListModel->index(0,0, fileListView->rootIndex()));
        //        QItemSelectionModel s;
        //        s.setCurrentIndex();
        //        QModelIndex idx = fileListModel->index(srcImageDir);
        QModelIndex m = fileListView->model()->index(1,0,fileListView->rootIndex());
        fileListView->selectionModel()->setCurrentIndex(m, QItemSelectionModel::Rows | QItemSelectionModel::Select);

        connect(fileListView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::onFileSelected);
        loadClassNames(srcImageDir+"/names.txt");
        _viewScene->classNameList = _classNames;
        _viewScene->installEventFilter(this);
    }
}
void MainWindow::loadClassNames(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    _classNames = new QList<QString>();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString s = in.readLine();
        if(!(s.simplified().isEmpty())) {
            _classNames->append(s);
        }
    }
    file.close();
}
void MainWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void MainWindow::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        //        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
                .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
                       tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
                          "and QScrollArea to display an image. QLabel is typically used "
                          "for displaying a text, but it can also display an image. "
                          "QScrollArea provides a scrolling view around another widget. "
                          "If the child widget exceeds the size of the frame, QScrollArea "
                          "automatically provides scroll bars. </p><p>The example "
                          "demonstrates how QLabel's ability to scale its contents "
                          "(QLabel::scaledContents), and QScrollArea's ability to "
                          "automatically resize its contents "
                          "(QScrollArea::widgetResizable), can be used to implement "
                          "zooming and scaling features. </p><p>In addition the example "
                          "shows how to use QPainter to print an image.</p>"));
}

/**
 * @brief MainWindow::createActions add menu actions
 */
void MainWindow::createActions()
{
    // file menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    // open folder
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/folder.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open Folder..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an image folder"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFolder);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // quit
    const QIcon exitIcon = QIcon::fromTheme("application-exit", QIcon(":/images/quit.png"));
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
    fileToolBar->addAction(exitAct);

    menuBar()->addSeparator();

    // edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    // draw
    const QIcon drawIcon = QIcon::fromTheme("document-open", QIcon(":/images/draw.png"));
    QAction *drawAct = new QAction(drawIcon, tr("&Draw"), this);
    drawAct->setShortcut(tr("Ctrl+D"));
    drawAct->setStatusTip(tr("Draw rectangle"));
    connect(drawAct, &QAction::triggered, this, &MainWindow::drawing);
    editMenu->addAction(drawAct);
    editToolBar->addAction(drawAct);

    // undo
    const QIcon undoIcon = QIcon::fromTheme("document-open", QIcon(":/images/undo.png"));
    QAction *undoAct = new QAction(undoIcon, tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the draw"));
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    // redo
    const QIcon redoIcon = QIcon::fromTheme("document-open", QIcon(":/images/redo.png"));
    QAction *redoAct = new QAction(redoIcon, tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Undo the draw"));
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    menuBar()->addSeparator();

    // view menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    QToolBar *viewToolBar = addToolBar(tr("View"));

    // zoom in
    const QIcon zoomInIcon = QIcon::fromTheme("document-open", QIcon(":/images/zoom-in.png"));
    QAction *zoomInAct = new QAction(zoomInIcon, tr("Zoom &In"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setStatusTip(tr("Zoom In Image"));
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);
    viewMenu->addAction(zoomInAct);
    viewToolBar->addAction(zoomInAct);

    // zoom out
    const QIcon zoomOutIcon = QIcon::fromTheme("document-open", QIcon(":/images/zoom-out.png"));
    QAction *zoomOutAct = new QAction(zoomOutIcon, tr("Zoom &Out"), this);
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
    QAction *actualSizeAct = new QAction(actualSizeIcon, tr("&Actual Size"), this);
    actualSizeAct->setShortcut(tr("Ctrl+R"));
    actualSizeAct->setStatusTip(tr("Fit View To Actual Size"));
    connect(actualSizeAct, &QAction::triggered, this, &MainWindow::fitViewToActual);
    viewMenu->addAction(actualSizeAct);
    viewToolBar->addAction(actualSizeAct);

    viewMenu->addSeparator();

    // full screen
    const QIcon fullscreenIcon = QIcon::fromTheme("document-open", QIcon(":/images/fullscreen.png"));
    QAction *fullscreenAct = new QAction(fullscreenIcon, tr("&Full Screen"), this);
    fullscreenAct->setShortcut(tr("Alt+Enter"));
    fullscreenAct->setStatusTip(tr("Full Screen"));
    connect(fullscreenAct, &QAction::triggered, this, &MainWindow::fullScreen);
    viewMenu->addAction(fullscreenAct);
    viewToolBar->addAction(fullscreenAct);

    // help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *helpToolBar = addToolBar(tr("Help"));

    //    // 语言
    //    QMenu *languageMenu = helpMenu->addMenu(
    //                QIcon::fromTheme("document-open", QIcon(":/images/language.png")), tr("&Language"));
    //    languageMenu->setStatusTip(tr("Select language"));

    //    // 中文
    //    QAction *zhCNAct = languageMenu->addAction(
    //                QIcon::fromTheme("document-open", QIcon(":/images/zh_CN.png")),
    //                tr("&Chinese"));
    //    languageMenu->addAction(zhCNAct);
    //    helpToolBar->addAction(zhCNAct);
    //    connect(zhCNAct, &QAction::triggered, this, &MainWindow::switchLanguage);

    //    // 英文
    //    QAction *enUSAct = languageMenu->addAction(
    //                QIcon::fromTheme("document-open", QIcon(":/images/en_US.png")),
    //                tr("&English"));
    //    languageMenu->addAction(enUSAct);
    //    helpToolBar->addAction(enUSAct);
    //    connect(zhCNAct, &QAction::triggered, this, &MainWindow::switchLanguage);

    //    // 帮助按钮
    //    QAction *helpAct = helpMenu->addAction(
    //                QIcon::fromTheme("document-open", QIcon(":/images/help.png")),
    //                tr("&Help"), this, &MainWindow::about);
    //    helpAct->setStatusTip(tr("Show the application's About box"));
    //    helpAct->setShortcut(QKeySequence::HelpContents);
    //    helpToolBar->addAction(helpAct);

    //    // 关于按钮
    //    QAction *aboutAct = helpMenu->addAction(
    //                QIcon::fromTheme("document-open", QIcon(":/images/about.png")),
    //                tr("&About"), qApp, &QApplication::aboutQt);
    //    aboutAct->setStatusTip(tr("Show the Qt library's About box"));
    //    helpToolBar->addAction(aboutAct);
}

void MainWindow::createCentralWindow()
{
    centralWidget = new QWidget(this);
    horizontalLayout = new QHBoxLayout(centralWidget);
    fileListView = new QTreeView(this);
    imageView = new QGraphicsView(this);
    imageView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    _viewScene = new ViewScene(this);

    fileListModel = new QFileSystemModel(this);
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.tif" << "*.tiff" << "*.bmp";

    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainSplitter->addWidget(fileListView);
    mainSplitter->addWidget(imageView);
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 8);
    mainSplitter->setOpaqueResize(false);

    horizontalLayout->addWidget(mainSplitter);
    centralWidget->setLayout(horizontalLayout);
    this->setCentralWidget(centralWidget);

}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_viewScene->isImageLoaded) {
        _viewScene->clear();
    }
}
void MainWindow::updateActions()
{
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::onFileSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (_viewScene->isImageLoaded) {
        _viewScene->clear();
    }
    auto index = selected.indexes().first();
    _viewScene->loadImage(fileListModel->filePath(index));

    imageView->setScene(_viewScene);
    isImageLoaded = true;
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
void MainWindow::drawing()
{
    if (!isImageLoaded)
        return;

//    _viewScene->isDrawing = true;
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

    //    int numDegrees = event->delta() / 8;
    //    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    //    _numScheduledScalings += numSteps;
    //    if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
    //    _numScheduledScalings = numSteps;

    //    QTimeLine *anim = new QTimeLine(350, this);
    //    anim->setUpdateInterval(20);

    //    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    //    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    //    anim->start();

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
