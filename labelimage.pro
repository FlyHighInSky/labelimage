QT += widgets


HEADERS       = \
    mainwindow.h \
    viewscene.h \
    boxitem.h \
    undohistory.h \
    FreeImage.h
SOURCES       = \
                main.cpp \
    mainwindow.cpp \
    viewscene.cpp \
    boxitem.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/labelimage
# INSTALLS += target

RESOURCES += \
    labelimage.qrc

win32: LIBS += -L$$PWD/lib/ -lFreeImage

DEPENDPATH += $$PWD/.
