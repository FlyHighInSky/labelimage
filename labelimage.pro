QT += widgets
VERSION = 1.1.0

HEADERS       = \
    mainwindow.h \
    boxitem.h \
    FreeImage.h \
    typeeditdialog.h \
    commands.h \
    customview.h \
    customscene.h
SOURCES       = \
                main.cpp \
    mainwindow.cpp \
    boxitem.cpp \
    typeeditdialog.cpp \
    commands.cpp \
    customview.cpp \
    customscene.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/labelimage
# INSTALLS += target

RESOURCES += \
    labelimage.qrc

win32: LIBS += -L$$PWD/lib/ -lFreeImage

DEPENDPATH += $$PWD/.

TRANSLATIONS += $$PWD/languages/zh_CN.ts \
               $$PWD/languages/en_US.ts

CODECFORSRC = UTF-8

win32: RC_ICONS += images/draw.ico
