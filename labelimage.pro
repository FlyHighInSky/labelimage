QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = labelimage.h
SOURCES       = labelimage.cpp \
                main.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/labelimage
# INSTALLS += target
