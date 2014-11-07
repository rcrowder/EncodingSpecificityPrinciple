#! [0]
TEMPLATE      = lib
CONFIG       += plugin static
QT           += widgets
INCLUDEPATH  += ../..
HEADERS       = basictoolsplugin.h
SOURCES       = basictoolsplugin.cpp
TARGET        = $$qtLibraryTarget(pnp_basictools)
DESTDIR       = %{sourceDir}/../../../cochlear-nerve/plugins
#! [0]

# install
target.path = %{sourceDir}/../../../cochlear-nerve/plugins
INSTALLS += target
