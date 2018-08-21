/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

//! [0]
MainWindow::MainWindow()
//   : imageLabel(new QLabel)
//   : scrollArea(new QScrollArea)
//   , scaleFactor(1)
{
//    imageLabel = new ImageLabelWidget();
//    imageLabel->setBackgroundRole(QPalette::Base);
//    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    imageLabel->setScaledContents(true);

//    scrollArea->setBackgroundRole(QPalette::Dark);
//    scrollArea->setWidget(imageLabel);
//    scrollArea->setVisible(false);
//    setCentralWidget(scrollArea);

    createActions();
    createCentralWindow();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
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

        fileListView->setRootIsDecorated(0);
        fileListView->setModel(fileListModel);
        fileListView->setRootIndex(fileListModel->index(srcImageDir));

    connect(fileListView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onFileSelected);
    }
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

//! [15]
void MainWindow::about()
//! [15] //! [16]
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
//! [16]

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
    scene = new ViewScene(this);

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

void MainWindow::updateActions()
{
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}


void MainWindow::onFileSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
    try {
        auto index = selected.indexes().first();
        scene->loadImage(fileListModel->filePath(index));
    } catch (int code) {
        return;
    }

    imageView->setScene(scene);
    isImageLoaded = true;
    fitViewToWindow();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    fitViewToWindow();
}

void MainWindow::zoomIn()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    scene->setViewZoom(scene->viewZoom() * 2);
}

void MainWindow::zoomOut()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    scene->setViewZoom(scene->viewZoom() * 0.5);
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

    auto oldZoom = scene->viewZoom();
    auto newZoom = oldZoom + (event->delta() / 120.0) * 0.05;
    scene->setViewZoom(newZoom <= 0 ? oldZoom : newZoom);
}

void MainWindow::fitViewToWindow()
{
    if (!isImageLoaded)
        return;
    if (!fitToWindowAct->isChecked())
        return;

    scene->setMaxViewSize(
                imageView->width() - 2 - 15,
                imageView->height() - 2 - 15 );
}

void MainWindow::fitViewToActual()
{
    if (!isImageLoaded)
        return;

    if (fitToWindowAct->isChecked())
        fitToWindowAct->setChecked(false);

    scene->setViewZoom(1);
}

void MainWindow::fullScreen()
{
    auto makeFullscreen = !isFullScreen();

    fileListView->setVisible(!makeFullscreen);
    setWindowState(makeFullscreen ? Qt::WindowFullScreen : Qt::WindowMaximized);
}
