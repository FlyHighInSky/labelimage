QT += widgets


HEADERS       = \
    mainwindow.h \
    viewscene.h \
    boxitem.h \
    undohistory.h \
    FreeImage.h \
    typeeditdialog.h
SOURCES       = \
                main.cpp \
    mainwindow.cpp \
    viewscene.cpp \
    boxitem.cpp \
    typeeditdialog.cpp

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
