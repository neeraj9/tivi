LIBS = -lm -lcurses
CPP = g++
CPPFLAGS = -g
LDFLAGS = -g -O2

HEADERS = eutil.h editor9.h

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: tivi

tivi: $(OBJECTS)
	$(CPP) $(LDFLAGS) $(LIBS) $(OBJECTS) -o $@

%.o: %.c $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -f *.o tivi

.PHONY: all clean
