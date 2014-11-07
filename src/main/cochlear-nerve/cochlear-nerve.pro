#! [0]
QT += widgets

INCLUDEPATH    += "../../../../../../../../Program Files/Mega-Nerd/libsndfile/include/"

HEADERS        = interfaces.h \
                 mainwindow.h \
                 paintarea.h \
                 plugindialog.h \
                 sndfile.h
SOURCES        = main.cpp \
                 mainwindow.cpp \
                 paintarea.cpp \
                 plugindialog.cpp

LIBS           = -L"../../../../../../../../Program Files/Mega-Nerd/libsndfile/lib" \
                 -L../cochlear-nerve/plugins \
                 -lpnp_basictools \
                 -lpnp_extrafilters \
                 -llibsndfile-1

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/cochlear-nerve
INSTALLS += target
