#! [0]
QT += widgets quick qml declarative

HEADERS        = interfaces.h \
                 mainwindow.h \
                 paintarea.h \
                 plugindialog.h

SOURCES        = main.cpp \
                 mainwindow.cpp \
                 paintarea.cpp \
                 plugindialog.cpp

RESOURCES     += main.qrc

LIBS           = -L../cochlear-nerve/plugins \
                 -lpnp_basictools

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/cochlear-nerve
INSTALLS += target

OTHER_FILES +=
