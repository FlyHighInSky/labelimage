#include <QApplication>
#include <QCommandLineParser>
#include "mainwindow.h"
#include "C:/Program Files (x86)/Visual Leak Detector/include/vld.h"
int main(int argc, char *argv[])
{
//    Q_IMPORT_PLUGIN( qtiff );
    QApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(MainWindow::tr("Image Labeler"));
    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(MainWindow::tr("[file]"), MainWindow::tr("Image file to open."));
    commandLineParser.process(QCoreApplication::arguments());

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
