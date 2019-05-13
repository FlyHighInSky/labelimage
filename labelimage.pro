TARGET = Image" "Labeler
QT += widgets
VERSION_MAJOR = 2
VERSION_MINOR = 1
VERSION_BUILD = 2


DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

HEADERS       = \
    mainwindow.h \
    boxitem.h \
    FreeImage.h \
    typeeditdialog.h \
    commands.h \
    customview.h \
    customscene.h \
    boxitemmimedata.h
SOURCES       = \
                main.cpp \
    mainwindow.cpp \
    boxitem.cpp \
    typeeditdialog.cpp \
    commands.cpp \
    customview.cpp \
    customscene.cpp \
    boxitemmimedata.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/labelimage
# INSTALLS += target

RESOURCES += \
    labelimage.qrc

win32 {
    LIBS += -L$$PWD/lib/ -lFreeImage
}
unix {
    LIBS += -L$$PWD/lib/ -lfreeimage -ltiff
    # install
    target.source = $$TARGET
    target.path = /usr/bin
    INSTALLS = target
}


DEPENDPATH += $$PWD/.

TRANSLATIONS += $$PWD/languages/zh_CN.ts \
               $$PWD/languages/en_US.ts

CODECFORSRC = UTF-8

win32 {
    RC_ICONS += images/draw.ico
}
