CFLAGS=-Wall -pedantic -std=gnu99 -g -I/usr/include `pkg-config --cflags cairo`
LDFLAGS=-Wall -g `pkg-config --libs cairo` -L/usr/lib -lX11

all: pmck

pmck: pmck.o
	cc -o pmck ${LDFLAGS} pmck.o

pmck.o: pmck.c
	cc -o pmck.o ${CFLAGS} -c pmck.c
