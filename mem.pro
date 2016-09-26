TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \ 
    main.cpp \
    main1.cpp \
    blankserver.c \
    fcntlserver.c \
    kkk.c \
    mai2.cpp \
    pureserver.c \
    pureclient.c \
    mai3.cpp \
    mai4.cpp \
    pure_server_1.c \
    pure_client_1.c \
    pure_client.c \
    pure_server.c \
    tmp_levent.cpp \
    lev_socket.c \
    tmpmem.cpp

HEADERS += \
    libmemcached/memcached.h \
    sk.h

DISTFILES += \
