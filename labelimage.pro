QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = \
    mainwindow.h \
    viewscene.h \
    boxitem.h \
    undohistory.h
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
