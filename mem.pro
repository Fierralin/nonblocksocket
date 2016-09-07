TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \ 
    main.cpp \
    main1.cpp \
    serv.cpp \
    blankserver.c \
    fcntlserver.c \
    kkk.c \
    lililili.c \
    lll.c \
    epoll_multiprocess.c \
    mai2.cpp \
    pureserver.c \
    pureclient.c

HEADERS += \
    libmemcached/memcached.h \
    sk.h

DISTFILES += \
