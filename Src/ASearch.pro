TARGET = pathplanning
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++14 -O3 -Wall -Wextra

win32 {
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}

SOURCES += \
    astar.cpp \
    config.cpp \
    dijkstra.cpp \
    environmentoptions.cpp \
    main.cpp \
    map.cpp \
    mission.cpp \
    search.cpp \
    tinyxml2.cpp \
    xmllogger.cpp 
     
HEADERS += \
    astar.h \
    auxiliary.h \
    config.h \
    dijkstra.h \
    environmentoptions.h \
    gl_const.h \
    ilogger.h \
    map.h \
    mission.h \
    node.h \
    search.h \
    searchresult.h \
    tinyxml2.h \
    xmllogger.h