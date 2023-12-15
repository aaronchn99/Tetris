LIBDIRS= -Lheaders -Llib
LDLIBS = -llibfreeglut -lopengl32 -lglu32

CPPFLAGS= -O3 -std=c++0x -Wl,--subsystem,windows
LDFLAGS= $(CPPFLAGS) $(LIBDIRS)

TARGETS = tetris

SRCS = tetris.cpp tetromino.h cube.h cross_product.h winstd.h

OBJS =  $(SRCS:.cpp=.o)

CXX = g++

default: $(TARGETS)
