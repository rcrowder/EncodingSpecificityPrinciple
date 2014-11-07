include(main.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG += ordered

!contains(DEFINES, DISABLE_FFT): SUBDIRS += ../../external/spectrum/3rdparty/fftreal
SUBDIRS += ../../external/spectrum/app
SUBDIRS += cochlear-nerve
SUBDIRS += cochlear-nerve-plugins

TARGET = main

