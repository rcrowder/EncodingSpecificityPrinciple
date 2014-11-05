#! [0]
TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
INCLUDEPATH  += ../..
HEADERS       = extrafiltersplugin.h
SOURCES       = extrafiltersplugin.cpp
TARGET        = $$qtLibraryTarget(pnp_extrafilters)
DESTDIR       = ../../cochlear-nerve/plugins

#! [0]
# install
target.path = ../../cochlear-nerve/plugins
INSTALLS += target
