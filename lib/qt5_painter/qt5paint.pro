QT += widgets

HEADERS       = qt5paint.h \
                scribblearea.h
SOURCES       = main.cpp \
                qt5paint.cpp \
                scribblearea.cpp

DEFINES += STANDALONE_BUILD
DEFINES += PAINTER_IS_A_MAINWINDOW

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/scribble
INSTALLS += target
