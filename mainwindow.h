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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QImage>
#include <QTranslator>
#ifndef QT_NO_PRINTER
#include <QTreeView>
#include <QGraphicsView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QHBoxLayout>
#include <QImageReader>
#endif
#include "viewscene.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openFolder();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void drawing();
    void fitViewToWindow();
    void fitViewToActual();
    void fullScreen();
    void about();
    void onFileSelected(const QItemSelection& selected, const QItemSelection& deselected);
    void updateLabelImageSize(QSize imageSize);
    void updateLabelCursorPos(QPointF cursorPos);
    void updateLabelBoxRect(QRect boxRect);
    void switchLanguage();

private:
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent* event);
    void createActions();
    void createCentralWindow();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void refresh();

    void loadClassNames(QString filePath);
    QWidget *centralWidget;
    QAction *fitToWindowAct;
    QImage image;
    QHBoxLayout *horizontalLayout;
    QSplitter *mainSplitter;
    QTreeView *fileListView;
    QGraphicsView *imageView;
    ViewScene *_viewScene = nullptr;
    QFileSystemModel *fileListModel;
    QStringList filters;
    QList<QString> *_classNames = nullptr;
    bool isImageLoaded =  false;
    QScrollArea *scrollArea;
    double scaleFactor;
    QLabel *_labelCursorPos, *_labelImageSize, *_labelBoxRect;
    QPointF _cursorPos;
    QSize _imageSize;
    QRect _boxRect;

    QAction *copyAct;
    QTranslator translator;

    QAction *zhCNAct, *enUSAct;
    QMenu *fileMenu;
    QToolBar *fileToolBar;
    QAction *openAct;
    QAction *exitAct;
    QMenu *editMenu;
    QToolBar *editToolBar;
    QAction *drawAct;
    QAction *undoAct;
    QAction *redoAct;
    QMenu *viewMenu;
    QToolBar *viewToolBar;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *actualSizeAct;
    QAction *fullscreenAct;
    QMenu *helpMenu;
    QToolBar *helpToolBar;
    QMenu *languageMenu;
    QAction *helpAct;
    QAction *aboutAct;
};
//! [0]

#endif
