QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = \
    imagelabelwidget.h \
    mainwindow.h \
    UndoHistory.h \
    imagelabeler.h \
    labelregion.h \
    viewscene.h \
    ImageLabeler.h \
    boxitem.h
SOURCES       = \
                main.cpp \
    imagelabelwidget.cpp \
    mainwindow.cpp \
    imagelabeler.cpp \
    labelregion.cpp \
    viewscene.cpp \
    boxitem.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/labelimage
# INSTALLS += target

RESOURCES += \
    labelimage.qrc
