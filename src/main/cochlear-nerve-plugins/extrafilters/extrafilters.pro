#! [0]
TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
INCLUDEPATH  += ../..
HEADERS       = extrafiltersplugin.h
SOURCES       = extrafiltersplugin.cpp
TARGET        = $$qtLibraryTarget(pnp_extrafilters)
DESTDIR       = %{sourceDir}/../../../cochlear-nerve/plugins
#! [0]

# install
target.path = %{sourceDir}/../../../cochlear-nerve/plugins
INSTALLS += target
