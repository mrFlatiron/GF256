TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        tests/main.cpp \
        tests/run_suits.cpp \
    gf256-3rd-party/gf256.cpp

HEADERS += \
    GF256/GF256.hpp \
    GF256/impl/representations.hpp \
    tests/run_suits.hpp \
    gf256-3rd-party/gf256.h

QMAKE_CXXFLAGS += -msse4.1
QMAKE_CXXFLAGS += -fno-inline

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
